/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/render_pass.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <iterator>
#include <mce/exceptions.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

render_pass::render_pass(device& device_, destruction_queue_manager* dqm,
						 std::shared_ptr<subpass_graph> subpasses,
						 std::shared_ptr<framebuffer_config> fb_config,
						 vk::ArrayProxy<attachment_access> attachment_access_modes)
		: device_(device_), subpasses_{std::move(subpasses)}, fb_config_{std::move(fb_config)} {

	if(attachment_access_modes.size() != fb_config_->attachment_configs().size()) {
		throw mce::graphics_exception(
				"Mismatching numbers of attachment layouts and attachment access modes.");
	}

	boost::container::small_vector<vk::AttachmentDescription, 16> att_desc;
	for(uint32_t i = 0; i < fb_config_->attachment_configs().size(); ++i) {
		vk::AttachmentDescription ad;
		ad.flags = fb_config_->attachment_configs()[i].flags();
		ad.format = fb_config_->attachment_configs()[i].format();
		ad.samples = vk::SampleCountFlagBits::e1;
		ad.initialLayout = attachment_access_modes.data()[i].initial_layout;
		ad.finalLayout = attachment_access_modes.data()[i].final_layout;
		ad.loadOp = attachment_access_modes.data()[i].load_op;
		ad.storeOp = attachment_access_modes.data()[i].store_op;
		ad.stencilLoadOp = attachment_access_modes.data()[i].stencil_load_op;
		ad.stencilStoreOp = attachment_access_modes.data()[i].stencil_store_op;
		att_desc.push_back(ad);
	}
	if(std::any_of(subpasses_->subpasses().begin(), subpasses_->subpasses().end(),
				   [](const subpass_entry& e) {
					   return !((e.color.size() == e.resolve.size()) || (e.resolve.size() == 0));
				   })) {
		throw mce::graphics_exception("Subpass has number of resolve attachments that neither matches the "
									  "number of color attachments nor is 0.");
	}
	boost::container::small_vector<vk::SubpassDescription, 16> subpass_desc;
	subpass_desc.reserve(subpasses_->subpasses().size());
	std::transform(subpasses_->subpasses().begin(), subpasses_->subpasses().end(),
				   std::back_inserter(subpass_desc), [](const subpass_entry& e) {
					   return vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics,
													 uint32_t(e.input.size()), e.input.data(),
													 uint32_t(e.color.size()), e.color.data(),
													 e.resolve.size() ? e.resolve.data() : nullptr,
													 e.depth_stencil ? &(e.depth_stencil.value()) : nullptr,
													 uint32_t(e.preserve.size()), e.preserve.data());
				   });

	native_render_pass_ = queued_handle<vk::UniqueRenderPass>(
			device_.native_device().createRenderPassUnique(vk::RenderPassCreateInfo(
					{}, uint32_t(att_desc.size()), att_desc.data(), uint32_t(subpass_desc.size()),
					subpass_desc.data(), uint32_t(subpasses_->dependencies().size()),
					subpasses->dependencies().data())),
			dqm);
}

render_pass::~render_pass() {}

void render_pass::begin(vk::CommandBuffer cb, framebuffer_frame& fb,
						vk::ArrayProxy<vk::ClearValue> clear_values,
						vk::SubpassContents subpass_contents) const {
	cb.beginRenderPass(vk::RenderPassBeginInfo(native_render_pass_.get(), fb.native_framebuffer(),
											   {{}, {fb.owner().size().x, fb.owner().size().y}},
											   clear_values.size(), clear_values.data()),
					   subpass_contents);
}

} /* namespace graphics */
} /* namespace mce */
