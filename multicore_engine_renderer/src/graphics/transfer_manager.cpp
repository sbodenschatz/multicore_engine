/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/transfer_manager.hpp>
#include <utility>

namespace mce {
namespace graphics {

transfer_manager::transfer_manager(device& dev, device_memory_manager_interface& mm,
								   destruction_queue_manager* dqm, uint32_t ring_slots)
		: dev{dev}, mm{mm}, dqm{dqm}, ring_slots{ring_slots},
		  transfer_cmd_pool{dev, dev.transfer_queue_index().first, true, true},
		  ownership_cmd_pool{dev, dev.graphics_queue_index().first, true, true},
		  staging_buffer{dev, mm, dqm, 1 << 27, vk::BufferUsageFlagBits::eTransferSrc},
		  chunk_placer{staging_buffer.mapped_pointer(), staging_buffer.size()} {}

transfer_manager::~transfer_manager() {
	if(!dqm) dev.native_device().waitIdle();
}

void transfer_manager::record_buffer_copy(void* staging_ptr, size_t data_size, vk::Buffer dst_buffer,
										  vk::DeviceSize dst_offset,
										  util::callback_pool_function<void(vk::Buffer)> callback) {
	running_jobs[current_ring_index].push_back(
			buffer_transfer_job(data_size, staging_ptr, dst_buffer, dst_offset, std::move(callback)));
	staging_buffer.flush_mapped(dev.native_device());
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe | vk::PipelineStageFlagBits::eHost,
			vk::PipelineStageFlagBits::eTopOfPipe, {}, {},
			{vk::BufferMemoryBarrier(vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead,
									 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
									 staging_buffer.native_buffer(), 0, VK_WHOLE_SIZE),
			 vk::BufferMemoryBarrier(~vk::AccessFlags{}, vk::AccessFlagBits::eTransferWrite,
									 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, dst_buffer, 0,
									 VK_WHOLE_SIZE)},
			{});
	transfer_command_bufers[current_ring_index]->copyBuffer(
			staging_buffer.native_buffer(), dst_buffer,
			{{chunk_placer.to_offset(staging_ptr), dst_offset, data_size}});
}
void transfer_manager::record_image_copy(void* staging_ptr, size_t data_size, base_image& dst_img,
										 vk::ImageLayout final_layout,
										 decltype(image_transfer_job::regions) regions,
										 util::callback_pool_function<void(vk::Image)> callback) {
	running_jobs[current_ring_index].push_back(image_transfer_job(data_size, staging_ptr,
																  dst_img.native_image(), final_layout,
																  std::move(regions), std::move(callback)));
	boost::container::small_vector<vk::BufferImageCopy, 16> regions_transformed(regions.begin(),
																				regions.end());
	auto offset = chunk_placer.to_offset(staging_ptr);
	for(vk::BufferImageCopy& region : regions_transformed) {
		region.bufferOffset += offset;
	}
	staging_buffer.flush_mapped(dev.native_device());
	transfer_command_bufers[current_ring_index]->pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe | vk::PipelineStageFlagBits::eHost,
			vk::PipelineStageFlagBits::eTopOfPipe, {}, {},
			{vk::BufferMemoryBarrier(vk::AccessFlagBits::eHostWrite, vk::AccessFlagBits::eTransferRead,
									 VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
									 staging_buffer.native_buffer(), 0, VK_WHOLE_SIZE)},
			{dst_img.generate_transition(vk::ImageLayout::eTransferDstOptimal, ~vk::AccessFlags{},
										 vk::AccessFlagBits::eTransferWrite)});
	transfer_command_bufers[current_ring_index]->copyBufferToImage(
			staging_buffer.native_buffer(), dst_img.native_image(), vk::ImageLayout::eTransferDstOptimal,
			{uint32_t(regions_transformed.size()), regions_transformed.data()});
}

void transfer_manager::start_frame() {
	std::lock_guard<std::mutex> lock(manager_mutex);
	start_frame_internal((current_ring_index + 1) % ring_slots);
}
void transfer_manager::start_frame(uint32_t ring_index) {
	std::lock_guard<std::mutex> lock(manager_mutex);
	start_frame_internal(ring_index);
}

void transfer_manager::reallocate_buffer(size_t min_size) {
	buffer new_buffer(dev, mm, dqm, std::max(min_size * 4, staging_buffer.size()),
					  vk::BufferUsageFlagBits::eTransferSrc);
	util::ring_chunk_placer new_chunk_placer(new_buffer.mapped_pointer(), new_buffer.size());
	using std::swap;
	swap(staging_buffer, new_buffer);
	swap(chunk_placer, new_chunk_placer);
}

} /* namespace graphics */
} /* namespace mce */
