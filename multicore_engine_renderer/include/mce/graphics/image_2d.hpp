/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_2D_HPP_
#define MCE_GRAPHICS_IMAGE_2D_HPP_

#include <glm/glm.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <vulkan/vulkan.hpp>

#include <mce/graphics/device_memory_manager.hpp>

namespace mce {
namespace graphics {
class device;

class image {
	vk::Format format_;
	glm::uvec2 size_;
	vk::ImageUsageFlags usage_;
	vk::ImageLayout layout_;
	bool mutable_format_;
	vk::ImageTiling tiling_;
	uint32_t mip_levels_;
	uint32_t layers_;
	device_memory_handle<device_memory_manager> mem_handle_;
	vk::UniqueImage img_;

public:
	image(device& dev, device_memory_manager& mem_mgr, vk::Format format, glm::uvec2 size,
		  vk::ImageUsageFlags usage, vk::ImageLayout layout = vk::ImageLayout::eGeneral,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  uint32_t mip_levels = 1, uint32_t layers = 1);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_IMAGE_2D_HPP_ */
