/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/container/small_vector.hpp>
#include <cassert>
#include <mce/exceptions.hpp>
#include <mce/graphics/sync_utils.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <utility>
#include <vulkan/vk_format.h>

namespace mce {
namespace graphics {

size_t transfer_manager::image_alignment(const base_image& img) {
	VkFormatSize size;
	VkFormat fmt = static_cast<VkFormat>(img.format());
	vkGetFormatSize(fmt, &size);
	return size.blockSizeInBits >> 3;
}

transfer_manager::transfer_manager(device& dev, device_memory_manager_interface& mm, uint32_t ring_slots)
		: dev{dev}, mm{mm}, transfer_cmd_pool{dev, dev.transfer_queue_index().first, true, true},
		  ownership_cmd_pool{dev, dev.graphics_queue_index().first, true, true}, dqm{&dev, ring_slots},
		  ring_slots{ring_slots}, running_jobs{ring_slots},
		  staging_buffer(dev, mm, &dqm, 1 << 27, vk::BufferUsageFlagBits::eTransferSrc,
						 vk::MemoryPropertyFlagBits::eHostVisible),
		  chunk_placer{staging_buffer.mapped_pointer(), staging_buffer.size()},
		  staging_buffer_ends{ring_slots, nullptr} {
	transfer_command_bufers.reserve(ring_slots);
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		pending_ownership_command_buffers.reserve(ring_slots);
	}
	fences.reserve(ring_slots);
	for(uint32_t i = 0; i < ring_slots; ++i) {
		transfer_command_bufers.push_back(transfer_cmd_pool.allocate_primary_command_buffer());
		if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
			pending_ownership_command_buffers.push_back(queued_handle<vk::UniqueCommandBuffer>(
					ownership_cmd_pool.allocate_primary_command_buffer(), &dqm));
			pending_ownership_command_buffers.back()->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
			pending_ownership_command_buffers.back()->end();
		}
		fences.push_back(dev->createFenceUnique({vk::FenceCreateFlagBits::eSignaled}));
	}
}

transfer_manager::~transfer_manager() {
	dev->waitIdle();
	transfer_command_bufers.clear();
	pending_ownership_command_buffers.clear();
}

void transfer_manager::record_buffer_copy(void* staging_ptr, size_t data_size, vk::Buffer dst_buffer,
										  vk::DeviceSize dst_offset,
										  util::callback_pool_function<void(vk::Buffer)> callback) {
	running_jobs[current_ring_index].push_back(
			buffer_transfer_job(data_size, staging_ptr, dst_buffer, dst_offset, std::move(callback)));
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands | vk::PipelineStageFlagBits::eHost,
			vk::PipelineStageFlagBits::eTransfer, {}, {},
			{vk::BufferMemoryBarrier(~vk::AccessFlags{}, vk::AccessFlagBits::eTransferWrite,
									 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dst_buffer, 0,
									 VK_WHOLE_SIZE)},
			{});
	transfer_command_bufers[current_ring_index]->copyBuffer(
			staging_buffer.native_buffer(), dst_buffer,
			{{chunk_placer.to_offset(staging_ptr), dst_offset, data_size}});
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		buffer_queue_ownership_transfer(dst_buffer, transfer_command_bufers[current_ring_index].get(),
										pending_ownership_command_buffers[current_ring_index].get(),
										dev.transfer_queue_index().first, dev.graphics_queue_index().first,
										vk::PipelineStageFlagBits::eTransfer,
										vk::PipelineStageFlagBits::eAllCommands,
										vk::AccessFlagBits::eTransferWrite, ~vk::AccessFlags{});
	}
}
void transfer_manager::record_image_copy(void* staging_ptr, size_t data_size, vk::Image dst_img,
										 vk::ImageLayout final_layout, vk::ImageAspectFlags aspects,
										 uint32_t mip_levels, uint32_t layers, vk::ImageLayout old_layout,
										 vk::ArrayProxy<const vk::BufferImageCopy> regions,
										 util::callback_pool_function<void(vk::Image)> callback) {
	running_jobs[current_ring_index].push_back(image_transfer_job(data_size, staging_ptr, dst_img, 1,
																  final_layout, aspects, mip_levels, layers,
																  old_layout, {}, std::move(callback)));
	boost::container::small_vector<vk::BufferImageCopy, 16> regions_transformed(regions.begin(),
																				regions.end());
	auto offset = chunk_placer.to_offset(staging_ptr);
	for(vk::BufferImageCopy& region : regions_transformed) {
		region.bufferOffset += offset;
	}
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands | vk::PipelineStageFlagBits::eHost,
			vk::PipelineStageFlagBits::eTransfer, {}, {}, {},
			{base_image::generate_transition_native(dst_img, old_layout, vk::ImageLayout::eTransferDstOptimal,
													allowed_flags_for_layout(old_layout),
													vk::AccessFlagBits::eTransferWrite, aspects, mip_levels,
													layers)});
	transfer_command_bufers[current_ring_index]->copyBufferToImage(
			staging_buffer.native_buffer(), dst_img, vk::ImageLayout::eTransferDstOptimal,
			{uint32_t(regions_transformed.size()), regions_transformed.data()});
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {},
			{base_image::generate_transition_native(dst_img, vk::ImageLayout::eTransferDstOptimal,
													final_layout, vk::AccessFlagBits::eTransferWrite,
													allowed_flags_for_layout(final_layout), aspects,
													mip_levels, layers)});
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		image_queue_ownership_transfer(
				dst_img, final_layout, transfer_command_bufers[current_ring_index].get(),
				pending_ownership_command_buffers[current_ring_index].get(), dev.transfer_queue_index().first,
				dev.graphics_queue_index().first, vk::PipelineStageFlagBits::eBottomOfPipe,
				vk::PipelineStageFlagBits::eAllCommands, {}, allowed_flags_for_layout(final_layout), aspects);
	}
}

void transfer_manager::start_frame() {
	std::unique_lock<std::mutex> lock(manager_mutex);
	start_frame_internal((current_ring_index + 1) % ring_slots, std::move(lock));
}
void transfer_manager::start_frame(uint32_t ring_index) {
	std::unique_lock<std::mutex> lock(manager_mutex);
	start_frame_internal(ring_index, std::move(lock));
}

void transfer_manager::reallocate_buffer(size_t min_size) {
	staging_buffer.flush_mapped(dev.native_device());
	buffer new_buffer(dev, mm, &dqm, std::max(min_size * 4, staging_buffer.size()),
					  vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible);
	util::ring_chunk_placer new_chunk_placer(new_buffer.mapped_pointer(), new_buffer.size());
	using std::swap;
	swap(staging_buffer, new_buffer);
	swap(chunk_placer, new_chunk_placer);
	staging_buffer_ends.assign(ring_slots, nullptr);
}
void transfer_manager::start_frame_internal(uint32_t ring_index, std::unique_lock<std::mutex> lock) {
	in_frame = true;
	auto jobs = job_scratch_pad.get();
	current_ring_index = ring_index;
	auto nd = dev.native_device();
	auto fence_wait_res = nd.waitForFences({fences[current_ring_index].get()}, true, ~0ull);
	if(fence_wait_res == vk::Result::eTimeout) {
		throw mce::graphics_exception("waiting for fence timed out.");
	}
	dqm.cleanup_and_set_current(ring_index);
	transfer_command_bufers[current_ring_index]->reset({});
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		ready_ownership_command_buffers.push_back(
				std::move(pending_ownership_command_buffers[current_ring_index]));
		pending_ownership_command_buffers[current_ring_index] = queued_handle<vk::UniqueCommandBuffer>(
				ownership_cmd_pool.allocate_primary_command_buffer(), &dqm);
		pending_ownership_command_buffers[current_ring_index]->begin(
				{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	}
	std::swap(running_jobs[current_ring_index], *jobs);
	if(staging_buffer_ends[current_ring_index]) chunk_placer.free_to(staging_buffer_ends[current_ring_index]);
	transfer_command_bufers[current_ring_index]->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe | vk::PipelineStageFlagBits::eHost,
			vk::PipelineStageFlagBits::eAllCommands, {}, {},
			{vk::BufferMemoryBarrier(vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead,
									 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
									 staging_buffer.native_buffer(), 0, VK_WHOLE_SIZE)},
			{});
	process_waiting_jobs();
	lock.unlock();
	process_ready_callbacks(*jobs);
	jobs->clear();
}
void transfer_manager::end_frame() {
	std::unique_lock<std::mutex> lock(manager_mutex);
	in_frame = false;
	process_waiting_jobs();
	staging_buffer.flush_mapped(dev.native_device());
	staging_buffer_ends[current_ring_index] = chunk_placer.in_position();
	transfer_command_bufers[current_ring_index]->end();
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		pending_ownership_command_buffers[current_ring_index]->end();
	}
	vk::SubmitInfo si;
	si.commandBufferCount = 1;
	si.pCommandBuffers = &*transfer_command_bufers[current_ring_index];
	dev->resetFences({fences[current_ring_index].get()});
	dev.transfer_queue().submit(si, *fences[current_ring_index]);
}
void transfer_manager::process_waiting_jobs() {
	struct size_visitor : boost::static_visitor<size_t> {
		size_t operator()(buffer_transfer_job& job) const {
			return job.size;
		}
		size_t operator()(image_transfer_job& job) const {
			return job.size;
		}
	};
	std::sort(waiting_jobs.begin(), waiting_jobs.end(), [](transfer_job& job_a, transfer_job& job_b) {
		size_visitor v;
		return job_a.apply_visitor(v) > job_b.apply_visitor(v);
	});

	struct ptr_visitor : boost::static_visitor<const char*> {
		const char* operator()(boost::blank) const {
			return nullptr;
		}
		const char* operator()(const std::shared_ptr<const char>& ptr) const {
			return ptr.get();
		}
		const char* operator()(const containers::pooled_byte_buffer_ptr& ptr) const {
			return ptr.data();
		}
	};

	struct job_visitor : boost::static_visitor<bool> {
		transfer_manager& mgr;
		ptr_visitor pv;
		bool operator()(buffer_transfer_job& job) const {
			if(!mgr.chunk_placer.can_fit(job.size)) return false;
			auto data = job.src_data.apply_visitor(pv);
			assert(data);
			auto staging_ptr = mgr.chunk_placer.place_chunk(data, job.size);
			mgr.record_buffer_copy(staging_ptr, job.size, job.dst_buffer, job.dst_offset,
								   std::move(job.completion_callback));
			return true;
		}
		bool operator()(image_transfer_job& job) const {
			if(!mgr.chunk_placer.can_fit(job.size, job.alignment)) return false;
			auto data = job.src_data.apply_visitor(pv);
			assert(data);
			auto staging_ptr = mgr.chunk_placer.place_chunk(data, job.size, job.alignment);
			mgr.record_image_copy(staging_ptr, job.size, job.dst_img, job.final_layout, job.aspects,
								  job.mip_levels, job.layers, job.old_layout, job.regions,
								  std::move(job.completion_callback));
			return true;
		}
		explicit job_visitor(transfer_manager& mgr) : mgr{mgr} {}
	};
	job_visitor v{*this};

	while(!waiting_jobs.empty()) {
		if(!waiting_jobs.back().apply_visitor(v)) break;
		waiting_jobs.pop_back();
	}
}
void transfer_manager::process_ready_callbacks(std::vector<transfer_job>& jobs) {
	struct job_visitor : boost::static_visitor<> {
		void operator()(buffer_transfer_job& job) const {
			if(job.completion_callback) job.completion_callback(job.dst_buffer);
		}
		void operator()(image_transfer_job& job) const {
			if(job.completion_callback) job.completion_callback(job.dst_img);
		}
	};
	job_visitor v;
	for(transfer_job& job : jobs) {
		job.apply_visitor(v);
	}
}

std::vector<boost::variant<queued_handle<vk::UniqueCommandBuffer>, vk::CommandBuffer>>
transfer_manager::retrieve_ready_ownership_transfers() {
	std::vector<boost::variant<queued_handle<vk::UniqueCommandBuffer>, vk::CommandBuffer>> res;
	std::unique_lock<std::mutex> lock(manager_mutex);
	if(dev.graphics_queue_index().first != dev.transfer_queue_index().first) {
		using std::swap;
		swap(ready_ownership_command_buffers, res);
	}
	return res;
}

} /* namespace graphics */
} /* namespace mce */
