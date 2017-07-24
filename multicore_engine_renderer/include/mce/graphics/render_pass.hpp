/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/render_pass.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_RENDER_PASS_HPP_
#define GRAPHICS_RENDER_PASS_HPP_

#include <mce/graphics/framebuffer_layout.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;
class window;

class render_pass {
private:
	device& device_;
	vk::UniqueRenderPass native_render_pass;
	std::shared_ptr<framebuffer_layout> fb_layout_;

public:
	render_pass(device& device_, std::shared_ptr<framebuffer_layout> fb_layout);
	~render_pass();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_RENDER_PASS_HPP_ */
