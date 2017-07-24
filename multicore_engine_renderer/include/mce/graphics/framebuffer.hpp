/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_FRAMEBUFFER_HPP_
#define GRAPHICS_FRAMEBUFFER_HPP_

#include <mce/graphics/framebuffer_layout.hpp>
#include <mce/graphics/image.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class framebuffer_frame {
	uint32_t swapchain_image_index;
	vk::UniqueFramebuffer native_framebuffer_;
};

class framebuffer {
private:
	device* dev_;
	window* win_;
	glm::uvec2 size_;
	std::vector<image_var> additional_attachments_;
	std::vector<image_view_var> attachment_views_;
	std::shared_ptr<framebuffer_layout> layout_;
	std::vector<framebuffer_frame> frames;

public:
	framebuffer(device& dev, window& win, std::shared_ptr<framebuffer_layout> layout);
	~framebuffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
