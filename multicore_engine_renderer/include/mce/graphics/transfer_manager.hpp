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
		// cppcheck-suppress passedByValue
		buffer_transfer_job(std::shared_ptr<const char> src_data, size_t size, void* staging_buffer_ptr,
							vk::Buffer dst_buffer, vk::DeviceSize dst_offset, no_callback_tag)
				: src_data{std::move(src_data)}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset} {}
		// cppcheck-suppress passedByValue
		buffer_transfer_job(containers::pooled_byte_buffer_ptr src_data, size_t size,
							void* staging_buffer_ptr, vk::Buffer dst_buffer, vk::DeviceSize dst_offset,
							no_callback_tag)
				: src_data{std::move(src_data)}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset} {}
		// cppcheck-suppress passedByValue
		buffer_transfer_job(size_t size, void* staging_buffer_ptr, vk::Buffer dst_buffer,
							vk::DeviceSize dst_offset, no_callback_tag)
				: src_data{boost::blank{}}, size{size}, staging_buffer_ptr{staging_buffer_ptr},
				  dst_buffer{dst_buffer}, dst_offset{dst_offset} {}
	};

	struct image_transfer_job {
		boost::variant<boost::blank, std::shared_ptr<const char>, containers::pooled_byte_buffer_ptr>
				src_data;
		size_t size = 0;
		void* staging_buffer_ptr = nullptr;
		vk::Image dst_img;
		vk::ImageLayout final_layout;
		boost::container::small_vector<vk::BufferImageCopy, 16> regions;
		util::callback_pool_function<void(vk::Image)> completion_callback;

		image_transfer_job(std::shared_ptr<const char> src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()},
				  completion_callback{std::move(completion_callback)} {}
		image_transfer_job(containers::pooled_byte_buffer_ptr src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()},
				  completion_callback{std::move(completion_callback)} {}
		image_transfer_job(void* staging_buffer_ptr, vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions,
						   util::callback_pool_function<void(vk::Image)> completion_callback)
				: src_data{boost::blank{}}, size{0}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()},
				  completion_callback{std::move(completion_callback)} {}
		image_transfer_job(std::shared_ptr<const char> src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions, no_callback_tag)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()} {}
		image_transfer_job(containers::pooled_byte_buffer_ptr src_data, size_t size, void* staging_buffer_ptr,
						   vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions, no_callback_tag)
				: src_data{src_data}, size{size}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()} {}
		image_transfer_job(void* staging_buffer_ptr, vk::Image dst_img, vk::ImageLayout final_layout,
						   vk::ArrayProxy<vk::BufferImageCopy> regions, no_callback_tag)
				: src_data{boost::blank{}}, size{0}, staging_buffer_ptr{staging_buffer_ptr}, dst_img{dst_img},
				  final_layout{final_layout}, regions{regions.begin(), regions.end()} {}
	};

	using transfer_job = boost::variant<buffer_transfer_job, image_transfer_job>;

	device& dev;
	device_memory_manager_interface& mm;
	destruction_queue_manager* dqm;
	uint32_t current_ring_index = 0;
	uint32_t ring_slots;
	std::vector<transfer_job> waiting_jobs;
	std::vector<std::vector<transfer_job>> running_jobs;
	command_pool transfer_cmd_pool;
	command_pool ownership_cmd_pool;
	std::vector<vk::UniqueCommandBuffer> transfer_command_bufers;
	std::vector<vk::UniqueCommandBuffer> pending_ownership_command_buffers;
	buffer staging_buffer;
	util::ring_chunk_placer chunk_placer;
	util::callback_pool completion_function_pool;
	containers::byte_buffer_pool byte_buff_pool;
	std::vector<void*> staging_buffer_ends;
	size_t immediate_allocation_slack = 128;
	mutable std::mutex manager_mutex;

	template <typename F>
	bool try_immediate_alloc_buffer(void* data, size_t data_size, vk::Buffer dst_buffer,
									vk::DeviceSize dst_offset, F&& callback) {
		if(chunk_placer.can_fit_no_wrap(data_size) ||
		   (chunk_placer.can_fit(data_size) &&
			chunk_placer.available_space_no_wrap() < immediate_allocation_slack)) {
			auto staging_ptr = chunk_placer.place_chunk(data, data_size);
			running_jobs[current_ring_index].push_back(buffer_transfer_job(
					data_size, staging_ptr, dst_buffer, dst_offset, std::forward<F>(callback)));
			transfer_command_bufers[current_ring_index]->copyBuffer(
					staging_buffer.native_buffer(), dst_buffer,
					{{chunk_placer.to_offset(staging_ptr), dst_offset, data_size}});
			return true;
		} else if(data_size > chunk_placer.buffer_space_size()) {
			// TODO Reallocate buffer
		} else
			return false;
	}

	void start_frame_internal(uint32_t ring_index);

public:
	transfer_manager(device& dev, device_memory_manager_interface& mm, destruction_queue_manager* dqm,
					 uint32_t ring_slots);
	~transfer_manager();

	void start_frame();
	void start_frame(uint32_t ring_index);
	void end_frame();

	template <typename F>
	void upload_buffer(void* data, size_t data_size, vk::Buffer dst_buffer, vk::DeviceSize dst_offset,
					   F&& callback) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data, data_size, dst_buffer, dst_offset, std::forward<F>(callback))) {
			auto byte_buff = byte_buff_pool.allocate_buffer(data_size);
			memcpy(byte_buff, data, data_size);
			waiting_jobs.push_back(buffer_transfer_job(std::move(byte_buff), data_size, nullptr, dst_buffer,
													   dst_offset, std::forward<F>(callback)));
		}
	}
	template <typename F>
	void upload_buffer(containers::pooled_byte_buffer_ptr data, size_t data_size, vk::Buffer dst_buffer,
					   vk::DeviceSize dst_offset, F&& callback) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data, data_size, dst_buffer, dst_offset, std::forward<F>(callback))) {
			waiting_jobs.push_back(buffer_transfer_job(std::move(data), data_size, nullptr, dst_buffer,
													   dst_offset, std::forward<F>(callback)));
		}
	}
	template <typename F>
	void upload_buffer(const std::shared_ptr<void>& data, size_t data_size, vk::Buffer dst_buffer,
					   vk::DeviceSize dst_offset, F&& callback) {
		std::lock_guard<std::mutex> lock(manager_mutex);
		if(!try_immediate_alloc_buffer(data, data_size, dst_buffer, dst_offset, std::forward<F>(callback))) {
			waiting_jobs.push_back(buffer_transfer_job(std::move(data), data_size, nullptr, dst_buffer,
													   dst_offset, std::forward<F>(callback)));
		}
	}

	// TODO: Handle multiple mip levels and trigger completion callback only after all of them are finished.
	template <typename F>
	void upload_image(void* data, size_t data_size, base_image& dst_img, vk::ImageLayout final_layout,
					  vk::ArrayProxy<vk::BufferImageCopy> regions, F&& callback);
	template <typename F>
	void upload_image(const std::shared_ptr<void>& data, size_t data_size, base_image& dst_img,
					  vk::ImageLayout final_layout, vk::ArrayProxy<vk::BufferImageCopy> regions,
					  F&& callback);
	template <typename F>
	void upload_image(containers::pooled_byte_buffer_ptr data, size_t data_size, base_image& dst_img,
					  vk::ImageLayout final_layout, vk::ArrayProxy<vk::BufferImageCopy> regions,
					  F&& callback);

	// TODO: Ownership transfer
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TRANSFER_MANAGER_HPP_ */
