/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_FRAMEBUFFER_HPP_
#define GRAPHICS_FRAMEBUFFER_HPP_

#include <mce/graphics/image.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class framebuffer {
private:
	device& dev_;
	vk::Image swapchain_image_;
	std::vector<image_2d> additional_attachments_;
	std::vector<image_view_2d> attachment_views_;
	vk::UniqueFramebuffer native_framebuffer_;

public:
	framebuffer(device& dev, uint32_t width, uint32_t height, int swapchain_image_index,
				std::vector<vk::Format> additional_attachment_formats);
	framebuffer(device& dev, uint32_t width, uint32_t height,
				std::vector<vk::Format> additional_attachment_formats);
	~framebuffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
