/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/framebuffer_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_FRAMEBUFFER_LAYOUT_HPP_
#define MCE_GRAPHICS_FRAMEBUFFER_LAYOUT_HPP_

#include <mce/graphics/image.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

struct framebuffer_attachment_layout {
	vk::Format format;
	image_aspect_mode aspect_mode;
	vk::AttachmentDescriptionFlags flags;
	bool swapchain_image = false;
};

struct framebuffer_layout {
	std::vector<framebuffer_attachment_layout> additional_attachment_layouts;
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FRAMEBUFFER_LAYOUT_HPP_ */
