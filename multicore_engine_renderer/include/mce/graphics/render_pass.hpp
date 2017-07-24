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

struct attachment_access {
	vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined;
	vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR;
	vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eDontCare;
	vk::AttachmentLoadOp store_op = vk::AttachmentLoadOp::eDontCare;
	vk::AttachmentLoadOp stencil_load_op = vk::AttachmentLoadOp::eDontCare;
	vk::AttachmentLoadOp stencil_store_op = vk::AttachmentLoadOp::eDontCare;
};

struct attachment_reference_layout {
	uint32_t subpass;
	uint32_t attachment;
	vk::ImageLayout layout;
};

class render_pass {
private:
	device& device_;
	vk::UniqueRenderPass native_render_pass;
	std::shared_ptr<framebuffer_layout> fb_layout_;

public:
	render_pass(device& device_, std::shared_ptr<framebuffer_layout> fb_layout,
				vk::ArrayProxy<attachment_access> attachment_access_modes,
				vk::ArrayProxy<attachment_reference_layout> attachment_ref_layout_map);
	~render_pass();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_RENDER_PASS_HPP_ */
