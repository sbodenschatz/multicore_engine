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
						 // cppcheck-suppress passedByValue
						 std::shared_ptr<const framebuffer_config> config,
						 // cppcheck-suppress passedByValue
						 std::vector<vk::RenderPass> compatible_passes)
		: dev_{&dev}, win_{&win}, size_{win.glfw_window().framebuffer_size()}, config_{std::move(config)} {
	if(std::count_if(config_->attachment_configs().begin(), config_->attachment_configs().end(),
					 [](const framebuffer_attachment_config& cfg) { return cfg.is_swapchain_image(); }) > 1)
		throw mce::graphics_exception(
				"Framebuffer config contains multiple swapchain image attachment configs.");
	for(auto& ac : config_->attachment_configs()) {
		if(ac.is_swapchain_image()) {
			attachments_.emplace_back(vk::Image());
			attachment_views_.emplace_back(vk::ImageView());
		} else {
			switch(ac.aspect_mode()) {
			case image_aspect_mode::color:
				attachments_.emplace_back(image_2d(dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
												   vk::ImageUsageFlagBits::eColorAttachment |
														   vk::ImageUsageFlagBits::eInputAttachment |
														   vk::ImageUsageFlagBits::eSampled));
				break;
			case image_aspect_mode::depth:
				attachments_.emplace_back(image_2d_depth(
						dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
						vk::ImageUsageFlagBits::eDepthStencilAttachment |
								vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled));
				break;
			case image_aspect_mode::depth_stencil:
				attachments_.emplace_back(image_2d_ds(
						dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
						vk::ImageUsageFlagBits::eDepthStencilAttachment |
								vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled));
				break;
			case image_aspect_mode::stencil:
				attachments_.emplace_back(image_2d_stencil(
						dev, mem_mgr, destruction_manager, ac.format(), size_, 1,
						vk::ImageUsageFlagBits::eDepthStencilAttachment |
								vk::ImageUsageFlagBits::eInputAttachment | vk::ImageUsageFlagBits::eSampled));
				break;
			}
			imgview_visitor v(this);
			attachments_.back().apply_visitor(v);
		}
	}
	std::vector<vk::ImageView> views;
	views.reserve(attachment_views_.size());
	std::transform(attachment_views_.begin(), attachment_views_.end(), std::back_inserter(views),
				   [](image_view_var& view) {
					   view_visitor v;
					   return view.apply_visitor(v);
				   });

	std::vector<vk::ImageView> pass_views;
	for(uint32_t pass_i = 0; pass_i < config_->passes().size(); ++pass_i) {
		const auto& used_attachments = config_->passes()[pass_i].used_attachments();
		auto sci_pos_it = std::find_if(
				used_attachments.begin(), used_attachments.end(), [this](uint32_t attachment_index) {
					return config_->attachment_configs().at(attachment_index).is_swapchain_image();
				});
		std::vector<framebuffer_frame> frames;
		pass_views.clear();
		pass_views.reserve(used_attachments.size());
		std::transform(used_attachments.begin(), used_attachments.end(), std::back_inserter(pass_views),
					   [&views](uint32_t attachment_index) { return views.at(attachment_index); });
		if(sci_pos_it != used_attachments.end()) {
			auto sci_pos = std::distance(used_attachments.begin(), sci_pos_it);
			for(uint32_t index = 0; index < win.swapchain_image_views().size(); ++index) {
				pass_views[sci_pos] = win.swapchain_image_views()[index].get();
				frames.push_back(framebuffer_frame(
						index, queued_handle<vk::UniqueFramebuffer>(
									   dev->createFramebufferUnique(vk::FramebufferCreateInfo(
											   {}, compatible_passes.at(pass_i), uint32_t(pass_views.size()),
											   pass_views.data(), size_.x, size_.y, 1u)),
									   destruction_manager),
						*this, pass_i));
			}
		} else {
			frames.push_back(framebuffer_frame(
					0, queued_handle<vk::UniqueFramebuffer>(
							   dev->createFramebufferUnique(vk::FramebufferCreateInfo(
									   {}, compatible_passes.at(pass_i), uint32_t(pass_views.size()),
									   pass_views.data(), size_.x, size_.y, 1u)),
							   destruction_manager),
					*this, pass_i));
		}
		passes_.push_back(framebuffer_pass(std::move(frames)));
	}
}

framebuffer::~framebuffer() {}

} /* namespace graphics */
} /* namespace mce */
