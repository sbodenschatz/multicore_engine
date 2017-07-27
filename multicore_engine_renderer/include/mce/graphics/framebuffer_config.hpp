/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/framebuffer_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_
#define MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_

#include <mce/graphics/image.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

class framebuffer_attachment_config {
	vk::Format format_;
	image_aspect_mode aspect_mode_;
	vk::AttachmentDescriptionFlags flags_;
	bool is_swapchain_image_ = false;

	friend class window;

public:
	framebuffer_attachment_config(vk::Format format, image_aspect_mode aspect_mode = image_aspect_mode::color,
								  vk::AttachmentDescriptionFlags flags = {})
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

class framebuffer_config {
	std::vector<framebuffer_attachment_config> attachment_configs_;

	friend class window;

public:
	framebuffer_config(vk::ArrayProxy<framebuffer_attachment_config> attachment_configs)
			: attachment_configs_{attachment_configs.begin(), attachment_configs.end()} {}
	framebuffer_config(std::vector<framebuffer_attachment_config>&& attachment_configs)
			: attachment_configs_{std::move(attachment_configs)} {}

	const std::vector<framebuffer_attachment_config>& attachment_configs() const {
		return attachment_configs_;
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_ */
