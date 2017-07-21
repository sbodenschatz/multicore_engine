/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/render_pass.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {

render_pass::render_pass(device& device_, window& window_, const vk::Rect2D& render_area,
						 vk::Format depth_format)
		: device_(device_), render_area(render_area), depth_format(depth_format) {

	vk::AttachmentDescription attachments_desc[2];
	attachments_desc[0].initialLayout = vk::ImageLayout::eUndefined;
	attachments_desc[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
	attachments_desc[0].format = window_.surface_format();
	attachments_desc[0].samples = vk::SampleCountFlagBits::e1;
	attachments_desc[0].loadOp = vk::AttachmentLoadOp::eClear;
	attachments_desc[0].storeOp = vk::AttachmentStoreOp::eStore;
	attachments_desc[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachments_desc[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	attachments_desc[1].initialLayout = vk::ImageLayout::eUndefined;
	attachments_desc[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	attachments_desc[1].format = depth_format;
	attachments_desc[1].samples = vk::SampleCountFlagBits::e1;
	attachments_desc[1].loadOp = vk::AttachmentLoadOp::eClear;
	attachments_desc[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	attachments_desc[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachments_desc[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	vk::AttachmentReference color_ref(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depth_ref(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpasses[1];
	subpasses[0].pColorAttachments = &color_ref;
	subpasses[0].colorAttachmentCount = 1;
	subpasses[0].pDepthStencilAttachment = &depth_ref;
	subpasses[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

	vk::RenderPassCreateInfo render_pass_ci;
	render_pass_ci.pAttachments = attachments_desc;
	render_pass_ci.attachmentCount = sizeof(attachments_desc) / sizeof(attachments_desc[0]);
	render_pass_ci.pSubpasses = subpasses;
	render_pass_ci.subpassCount = sizeof(subpasses) / sizeof(subpasses[0]);

	native_render_pass = device_.native_device().createRenderPassUnique(render_pass_ci);
}

render_pass::~render_pass() {}

} /* namespace graphics */
} /* namespace mce */
