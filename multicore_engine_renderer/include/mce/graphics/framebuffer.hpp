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
	window& win_;
	glm::uvec2 size_;
	std::vector<image_var> additional_attachments_;
	std::vector<image_view_2d> attachment_views_;
	vk::UniqueFramebuffer native_framebuffer_;

public:
	framebuffer(device& dev, window& win,
				std::vector<vk::Format> additional_attachment_formats);
	~framebuffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
