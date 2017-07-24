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

class framebuffer_attachment_layout {
	vk::Format format_;
	image_aspect_mode aspect_mode_;
	vk::AttachmentDescriptionFlags flags_;
	bool is_swapchain_image_ = false;

public:
	framebuffer_attachment_layout(vk::Format format, image_aspect_mode aspect_mode,
								  vk::AttachmentDescriptionFlags flags)
			: format_{format}, aspect_mode_{aspect_mode}, flags_{flags} {}

	image_aspect_mode aspect_mode() const {
		return aspect_mode_;
	}

	vk::AttachmentDescriptionFlags flags() const {
		return flags_;
	}

	vk::Format format() const {
		return format_;
	}

	bool is_swapchain_image() const {
		return is_swapchain_image_;
	}
};

class framebuffer_layout {
	std::vector<framebuffer_attachment_layout> attachment_layouts_;

public:
	framebuffer_layout(vk::ArrayProxy<framebuffer_attachment_layout> attachment_layouts)
			: attachment_layouts_{attachment_layouts.begin(), attachment_layouts.end()} {}

	const std::vector<framebuffer_attachment_layout>& attachment_layouts() const {
		return attachment_layouts_;
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FRAMEBUFFER_LAYOUT_HPP_ */
