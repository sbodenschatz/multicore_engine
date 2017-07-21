/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/render_pass.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_RENDER_PASS_HPP_
#define GRAPHICS_RENDER_PASS_HPP_

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
	vk::Rect2D render_area;
	std::vector<vk::ClearValue> clear_values;
	vk::Format depth_format;

public:
	render_pass(device& device_, window& window_, const vk::Rect2D& render_area, vk::Format depth_format);
	~render_pass();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_RENDER_PASS_HPP_ */
