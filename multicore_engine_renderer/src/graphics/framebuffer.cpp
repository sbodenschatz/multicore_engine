/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/exceptions.hpp>
#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

framebuffer::framebuffer(device& dev, window& win, device_memory_manager_interface& mem_mgr,
						 destruction_queue_manager* destruction_manager,
						 std::shared_ptr<framebuffer_config> config, vk::RenderPass compatible_pass)
		: dev_{&dev}, win_{&win}, size_{win.glfw_window().framebuffer_size()}, config_{std::move(config)} {
	if(std::count_if(config_->attachment_configs().begin(), config_->attachment_configs().end(),
					 [](const framebuffer_attachment_config& cfg) { return cfg.is_swapchain_image(); }) > 1)
		throw mce::graphics_exception(
				"Framebuffer config contains multiple swapchain image attachment configs.");
	for(auto& ac : config_->attachment_configs()) {
		if(ac.is_swapchain_image()) {
			additional_attachments_.emplace_back(vk::Image());
			attachment_views_.emplace_back(vk::ImageView());
		} else {
			switch(ac.aspect_mode()) {
			case image_aspect_mode::color:
				additional_attachments_.emplace_back(image_2d(
						dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
						vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eInputAttachment |
								vk::ImageUsageFlagBits::eSampled));
				break;
			case image_aspect_mode::depth:
			case image_aspect_mode::depth_stencil:
			case image_aspect_mode::stencil:
				additional_attachments_.emplace_back(image_2d_ds(
						dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
						vk::ImageUsageFlagBits::eDepthStencilAttachment |
								vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled));
				break;
			}
			imgview_visitor v(this);
			additional_attachments_.back().apply_visitor(v);
		}
	}
	std::vector<vk::ImageView> views;
	std::transform(attachment_views_.begin(), attachment_views_.end(), std::back_inserter(views),
				   [](image_view_var& view) {
					   view_visitor v;
					   return view.apply_visitor(v);
				   });
	for(vk::Image sci : win.swapchain_images()) {
		// TODO: Create framebuffer_frame objects.
		UNUSED(sci);
		UNUSED(compatible_pass);
	}
}

framebuffer::~framebuffer() {}

} /* namespace graphics */
} /* namespace mce */
