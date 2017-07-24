/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/framebuffer.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

framebuffer::framebuffer(device& dev, uint32_t width, uint32_t height, int swapchain_image_index,
						 std::vector<vk::Format> additional_attachment_formats)
		: dev_(dev) {
	UNUSED(width);
	UNUSED(height);
	UNUSED(swapchain_image_index);
	UNUSED(additional_attachment_formats);
}
framebuffer::framebuffer(device& dev, uint32_t width, uint32_t height,
						 std::vector<vk::Format> additional_attachment_formats)
		: dev_(dev) {
	UNUSED(width);
	UNUSED(height);
	UNUSED(additional_attachment_formats);
}

framebuffer::~framebuffer() {}

} /* namespace graphics */
} /* namespace mce */
