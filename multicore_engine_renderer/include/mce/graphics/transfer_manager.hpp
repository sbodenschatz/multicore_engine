/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/transfer_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_TRANSFER_MANAGER_HPP_
#define MCE_GRAPHICS_TRANSFER_MANAGER_HPP_

#include <glm/glm.hpp>
#include <mce/graphics/buffer.hpp>
#include <mce/graphics/command_pool.hpp>
#include <mce/graphics/image.hpp>
#include <mce/util/callback_pool.hpp>
#include <mce/util/ring_chunk_placer.hpp>
#include <vector>

namespace mce {
namespace graphics {

class transfer_manager {
private:
	struct buffer_transfer_job {};

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

public:
	transfer_manager(device& dev, device_memory_manager_interface& mm, destruction_queue_manager* dqm,
					 uint32_t ring_slots);
	~transfer_manager();

	void advance();
	void complete_and_set_current(uint32_t ring_index);

	void upload_buffer(void* data, size_t data_size, vk::Buffer dst_buffer, vk::DeviceSize dst_offset);
	void upload_buffer(const std::shared_ptr<void>& data, size_t data_size, vk::Buffer dst_buffer,
					   vk::DeviceSize dst_offset);

	template <typename Img>
	void upload_single_image(void* data, Img& dst_img, vk::ImageLayout final_layout,
							 vk::ImageAspectFlags aspectMask, uint32_t mip_level,
							 typename Img::size_type image_offset, typename Img::size_type image_extent);

	template <typename Img>
	void upload_single_image(const std::shared_ptr<void>& data, Img& dst_img, vk::ImageLayout final_layout,
							 vk::ImageAspectFlags aspectMask, uint32_t mip_level,
							 typename Img::size_type image_offset, typename Img::size_type image_extent);

	template <typename Img>
	void upload_layered_image(void* data, Img& dst_img, vk::ImageLayout final_layout,
							  vk::ImageAspectFlags aspectMask, uint32_t mip_level, uint32_t base_array_layer,
							  uint32_t layer_count, typename Img::size_type image_offset,
							  typename Img::size_type image_extent);
	template <typename Img>
	void upload_layered_image(const std::shared_ptr<void>& data, Img& dst_img, vk::ImageLayout final_layout,
							  vk::ImageAspectFlags aspectMask, uint32_t mip_level, uint32_t base_array_layer,
							  uint32_t layer_count, typename Img::size_type image_offset,
							  typename Img::size_type image_extent);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_TRANSFER_MANAGER_HPP_ */
