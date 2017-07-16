/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_TRANSFER_MANAGER_HPP_
#define MCE_GRAPHICS_TRANSFER_MANAGER_HPP_

#include <boost/variant.hpp>
#include <glm/glm.hpp>
#include <mce/containers/byte_buffer_pool.hpp>
#include <mce/containers/scratch_pad_pool.hpp>
#include <mce/graphics/buffer.hpp>
#include <mce/graphics/command_pool.hpp>
#include <mce/graphics/image.hpp>
#include <mce/util/callback_pool.hpp>
#include <mce/util/ring_chunk_placer.hpp>
#include <mutex>
#include <vector>

namespace mce {
namespace graphics {

class transfer_manager {
public:
	struct no_callback_tag {};

private:
	struct buffer_transfer_job {
		boost::variant<boost::blank, std::shared_ptr<const char>, containers::pooled_byte_buffer_ptr>
				src_data;
		size_t size = 0;
		void* staging_buffer_ptr = nullptr;
		vk::Buffer dst_buffer;
		vk::DeviceSize dst_offset = 0;
		util::callback_pool_function<void(vk::Buffer)> completion_callback;

		// cppcheck-suppress passedByValue
		buffer_transfer_job(std::shared_ptr<const char> src_data, size_t size, void* staging_buffer_ptr,
							vk::Buffer dst_buffer, vk::DeviceSize dst_offset,
							util::callback_pool_function<void(vk::Buffer)> completion_callback)
				: src_data{std::move(src_data)}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset}, completion_callback{
																		  std::move(completion_callback)} {}
		// cppcheck-suppress passedByValue
		buffer_transfer_job(containers::pooled_byte_buffer_ptr src_data, size_t size,
							void* staging_buffer_ptr, vk::Buffer dst_buffer, vk::DeviceSize dst_offset,
							util::callback_pool_function<void(vk::Buffer)> completion_callback)
				: src_data{std::move(src_data)}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset}, completion_callback{
																		  std::move(completion_callback)} {}
		// cppcheck-suppress passedByValue
		buffer_transfer_job(size_t size, void* staging_buffer_ptr, vk::Buffer dst_buffer,
							vk::DeviceSize dst_offset,
							util::callback_pool_function<void(vk::Buffer)> completion_callback)
				: src_data{boost::blank{}}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset}, completion_callback{
																		  std::move(completion_callback)} {}
	};

	struct image_transfer_job {
		boost::variant<boost::blank, std::shared_ptr<const char>, containers::pooled_byte_buffer_ptr>
				src_data;
		size_t size = 0;
		void* staging_buffer_ptr = nullptr;
		vk::Image dst_img;
		vk::ImageLayout final_layout;
		vk::ImageAspectFlags aspects;
		uint32_t mip_levels;
		uint32_t layers;
		vk::ImageLayout old_layout;
		std::vector<vk::BufferImageCopy> regions; // TODO: Find nothrow but heap-avoiding solution.
		util::callback_pool_function<void(vk::Image)> completion_callback;

		image_transfer_job(std::shared_ptr<const char> src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout, vk::ImageAspectFlags aspects,
						   uint32_t mip_levels, uint32_t layers, vk::ImageLayout old_layout,
						   decltype(regions) regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, aspects{aspects}, mip_levels{mip_levels}, layers{layers},
				  old_layout{old_layout}, regions{std::move(regions)}, completion_callback{std::move(
																			   completion_callback)} {}
		image_transfer_job(containers::pooled_byte_buffer_ptr src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout, vk::ImageAspectFlags aspects,
						   uint32_t mip_levels, uint32_t layers, vk::ImageLayout old_layout,
						   decltype(regions) regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, aspects{aspects}, mip_levels{mip_levels}, layers{layers},
				  old_layout{old_layout}, regions{std::move(regions)}, completion_callback{std::move(
																			   completion_callback)} {}
		image_transfer_job(size_t size, void* staging_buffer_ptr, vk::Image dst_img,
						   vk::ImageLayout final_layout, vk::ImageAspectFlags aspects, uint32_t mip_levels,
						   uint32_t layers, vk::ImageLayout old_layout, decltype(regions) regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{boost::blank{}}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_img{dst_img}, final_layout{final_layout}, aspects{aspects},
				  mip_levels{mip_levels}, layers{layers}, old_layout{old_layout}, regions{std::move(regions)},
				  completion_callback{std::move(completion_callback)} {}

		image_transfer_job(image_transfer_job&& other) noexcept
				: src_data{std::move(other.src_data)}, size{other.size},
				  staging_buffer_ptr{std::move(other.staging_buffer_ptr)}, dst_img{other.dst_img},
				  final_layout{other.final_layout}, aspects{other.aspects}, mip_levels{other.mip_levels},
				  layers{other.layers}, old_layout{other.old_layout}, regions{std::move(other.regions)},
				  completion_callback{std::move(other.completion_callback)} {
			other.size = 0;
			other.dst_img = vk::Image{};
			other.final_layout = {};
			other.aspects = {};
			other.mip_levels = 0;
			other.layers = {};
			other.old_layout = {};
		}
		image_transfer_job& operator=(image_transfer_job&& other) noexcept {
			src_data = std::move(other.src_data);
			size = other.size;
			staging_buffer_ptr = std::move(other.staging_buffer_ptr);
			dst_img = other.dst_img;
			final_layout = other.final_layout;
			aspects = other.aspects;
			mip_levels = other.mip_levels;
			layers = other.layers;
			old_layout = other.old_layout;
			regions = std::move(other.regions);
			completion_callback = std::move(other.completion_callback);
			other.size = 0;
			other.dst_img = vk::Image{};
			other.final_layout = {};
			other.aspects = {};
			other.mip_levels = 0;
			other.layers = {};
			other.old_layout = {};
			return *this;
		}
	};

	using transfer_job = boost::variant<buffer_transfer_job, image_transfer_job>;

	device& dev;
	device_memory_manager_interface& mm;
	destruction_queue_manager dqm;
	uint32_t current_ring_index = 0;
	uint32_t ring_slots;
	std::vector<transfer_job> waiting_jobs;
	std::vector<std::vector<transfer_job>> running_jobs;
	command_pool transfer_cmd_pool;
	command_pool ownership_cmd_pool;
	std::vector<vk::UniqueCommandBuffer> transfer_command_bufers;
	std::vector<vk::UniqueCommandBuffer> pending_ownership_command_buffers;
	std::vector<vk::UniqueCommandBuffer> ready_ownership_command_buffers;
	buffer staging_buffer;
	util::ring_chunk_placer chunk_placer;
	util::callback_pool completion_function_pool;
	containers::byte_buffer_pool byte_buff_pool;
	std::vector<const void*> staging_buffer_ends;
	size_t immediate_allocation_slack = 128;
	std::vector<vk::UniqueFence> fences;
	containers::scratch_pad_pool<std::vector<transfer_job>> job_scratch_pad;
	mutable std::mutex manager_mutex;

	template <typename S, typename F>
	util::callback_pool_function<S> take_callback(F&& f) {
		return completion_function_pool.allocate_function<S>(std::forward<F>(f));
	}
	template <typename S>
	util::callback_pool_function<S> take_callback(no_callback_tag) {
		return util::callback_pool_function<S>();
	}

	void record_buffer_copy(void* staging_ptr, size_t data_size, vk::Buffer dst_buffer,
							vk::DeviceSize dst_offset,
							util::callback_pool_function<void(vk::Buffer)> callback);
	void record_image_copy(void* staging_ptr, size_t data_size, vk::Image dst_img,
						   vk::ImageLayout final_layout, vk::ImageAspectFlags aspects, uint32_t mip_levels,
						   uint32_t layers, vk::ImageLayout old_layout,
						   vk::ArrayProxy<const vk::BufferImageCopy> regions,
						   util::callback_pool_function<void(vk::Image)> callback);

	void reallocate_buffer(size_t min_size);
	void process_waiting_jobs();
	void process_ready_callbacks(std::vector<transfer_job>& jobs);

	template <typename F>
	bool try_immediate_alloc_buffer(const void* data, size_t data_size, vk::Buffer dst_buffer,
									vk::DeviceSize dst_offset, F&& callback) {
		if(data_size > chunk_placer.buffer_space_size()) {
			reallocate_buffer(data_size);
		}
		if(chunk_placer.can_fit_no_wrap(data_size) ||
		   (chunk_placer.can_fit(data_size) &&
			chunk_placer.available_space_no_wrap() < immediate_allocation_slack)) {
			auto staging_ptr = chunk_placer.place_chunk(data, data_size);
			record_buffer_copy(staging_ptr, data_size, dst_buffer, dst_offset,
							   take_callback<void(vk::Buffer)>(std::forward<F>(callback)));
			return true;
		} else
			return false;
	}

	template <typename F>
	bool try_immediate_alloc_image(const void* data, size_t data_size, base_image& dst_img,
								   vk::ImageLayout final_layout,
								   vk::ArrayProxy<const vk::BufferImageCopy> regions, F&& callback) {
		if(data_size > chunk_placer.buffer_space_size()) {
			reallocate_buffer(data_size);
		}
		if(chunk_placer.can_fit_no_wrap(data_size) ||
		   (chunk_placer.can_fit(data_size) &&
			chunk_placer.available_space_no_wrap() < immediate_allocation_slack)) {
			auto staging_ptr = chunk_placer.place_chunk(data, data_size);
			record_image_copy(staging_ptr, data_size, dst_img.native_image(), final_layout,
							  dst_img.default_aspect_flags(), dst_img.mip_levels(), dst_img.layers(),
							  dst_img.layout(), regions,
							  take_callback<void(vk::Image)>(std::forward<F>(callback)));
			return true;
		} else
			return false;
	}

	void start_frame_internal(uint32_t ring_index, std::unique_lock<std::mutex> lock);

public:
	transfer_manager(device& dev, device_memory_manager_interface& mm, uint32_t ring_slots);
	~transfer_manager();

	void start_frame();
	void start_frame(uint32_t ring_index);
	void end_frame();
	std::vector<vk::UniqueCommandBuffer> retrieve_ready_ownership_transfers();

	template <typename F = no_callback_tag>
	void upload_buffer(void* data, size_t data_size, vk::Buffer dst_buffer, vk::DeviceSize dst_offset,
					   F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data, data_size, dst_buffer, dst_offset, std::forward<F>(callback))) {
			auto byte_buff = byte_buff_pool.allocate_buffer(data_size);
			memcpy(byte_buff, data, data_size);
			waiting_jobs.push_back(
					buffer_transfer_job(std::move(byte_buff), data_size, nullptr, dst_buffer, dst_offset,
										take_callback<void(vk::Buffer)>(std::forward<F>(callback))));
		}
	}
	template <typename F = no_callback_tag>
	void upload_buffer(containers::pooled_byte_buffer_ptr data, size_t data_size, vk::Buffer dst_buffer,
					   vk::DeviceSize dst_offset, F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data.data(), data_size, dst_buffer, dst_offset,
									   std::forward<F>(callback))) {
			waiting_jobs.push_back(
					buffer_transfer_job(std::move(data), data_size, nullptr, dst_buffer, dst_offset,
										take_callback<void(vk::Buffer)>(std::forward<F>(callback))));
		}
	}
	template <typename F = no_callback_tag>
	void upload_buffer(const std::shared_ptr<const char>& data, size_t data_size, vk::Buffer dst_buffer,
					   vk::DeviceSize dst_offset, F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data.get(), data_size, dst_buffer, dst_offset,
									   std::forward<F>(callback))) {
			waiting_jobs.push_back(
					buffer_transfer_job(std::move(data), data_size, nullptr, dst_buffer, dst_offset,
										take_callback<void(vk::Buffer)>(std::forward<F>(callback))));
		}
	}

	template <typename F = no_callback_tag>
	void upload_image(void* data, size_t data_size, base_image& dst_img, vk::ImageLayout final_layout,
					  vk::ArrayProxy<const vk::BufferImageCopy> regions, F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_image(data, data_size, dst_img, final_layout, regions,
									  std::forward<F>(callback))) {
			auto byte_buff = byte_buff_pool.allocate_buffer(data_size);
			memcpy(byte_buff, data, data_size);
			waiting_jobs.push_back(image_transfer_job(
					byte_buff, data_size, nullptr, dst_img.native_image(), final_layout,
					dst_img.default_aspect_flags(), dst_img.mip_levels(), dst_img.layers(), dst_img.layout(),
					decltype(image_transfer_job::regions)(regions.begin(), regions.end()),
					take_callback<void(vk::Image)>(std::forward<F>(callback))));
		}
		dst_img.set_layout_external(final_layout);
	}
	template <typename F = no_callback_tag>
	void upload_image(const std::shared_ptr<const char>& data, size_t data_size, base_image& dst_img,
					  vk::ImageLayout final_layout, vk::ArrayProxy<const vk::BufferImageCopy> regions,
					  F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_image(data.get(), data_size, dst_img, final_layout, regions,
									  std::forward<F>(callback))) {
			waiting_jobs.push_back(image_transfer_job(
					data, data_size, nullptr, dst_img.native_image(), final_layout,
					dst_img.default_aspect_flags(), dst_img.mip_levels(), dst_img.layers(), dst_img.layout(),
					decltype(image_transfer_job::regions)(regions.begin(), regions.end()),
					take_callback<void(vk::Image)>(std::forward<F>(callback))));
		}
		dst_img.set_layout_external(final_layout);
	}
	template <typename F = no_callback_tag>
	void upload_image(containers::pooled_byte_buffer_ptr data, size_t data_size, base_image& dst_img,
					  vk::ImageLayout final_layout, vk::ArrayProxy<const vk::BufferImageCopy> regions,
					  F&& callback = no_callback_tag{}) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_image(data.data(), data_size, dst_img, final_layout, regions,
									  std::forward<F>(callback))) {
			waiting_jobs.push_back(image_transfer_job(
					std::move(data), data_size, nullptr, dst_img.native_image(), final_layout,
					dst_img.default_aspect_flags(), dst_img.mip_levels(), dst_img.layers(), dst_img.layout(),
					decltype(image_transfer_job::regions)(regions.begin(), regions.end()),
					take_callback<void(vk::Image)>(std::forward<F>(callback))));
		}
		dst_img.set_layout_external(final_layout);
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TRANSFER_MANAGER_HPP_ */
