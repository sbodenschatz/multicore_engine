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

class image {
	vk::Format format_;
	glm::uvec2 size_;
	vk::ImageUsageFlags usage_;
	vk::ImageTiling tiling_;
	vk::ImageLayout layout_;
	uint32_t mip_levels_;
	uint32_t layers_;
	vk::UniqueImage img_;
	device_memory_handle<device_memory_manager> mem_handle_;

public:
	image();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_IMAGE_2D_HPP_ */
