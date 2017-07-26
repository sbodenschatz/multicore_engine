/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/render_pass.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_RENDER_PASS_HPP_
#define GRAPHICS_RENDER_PASS_HPP_

#include <boost/optional.hpp>
#include <mce/graphics/framebuffer.hpp>
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
	vk::AttachmentStoreOp store_op = vk::AttachmentStoreOp::eDontCare;
	vk::AttachmentLoadOp stencil_load_op = vk::AttachmentLoadOp::eDontCare;
	vk::AttachmentStoreOp stencil_store_op = vk::AttachmentStoreOp::eDontCare;
};

enum class attachment_ref_type { input, color, resolve, depth_stencil, preserve };

struct subpass_entry {
	vk::PipelineBindPoint pipeline_bind_point;
	std::vector<vk::AttachmentReference> input;
	std::vector<vk::AttachmentReference> color;
	std::vector<vk::AttachmentReference> resolve;
	boost::optional<vk::AttachmentReference> depth_stencil;
	std::vector<uint32_t> preserve;
};

class subpass_graph {
	std::vector<subpass_entry> subpasses_;
	std::vector<vk::SubpassDependency> dependencies_;

public:
	subpass_graph(std::vector<subpass_entry> subpasses, std::vector<vk::SubpassDependency> dependencies)
			: subpasses_{std::move(subpasses)}, dependencies_{std::move(dependencies)} {}

	const std::vector<vk::SubpassDependency>& dependencies() const {
		return dependencies_;
	}

	const std::vector<subpass_entry>& subpasses() const {
		return subpasses_;
	}
};

class render_pass {
private:
	device& device_;
	queued_handle<vk::UniqueRenderPass> native_render_pass_;
	std::shared_ptr<subpass_graph> subpasses_;
	std::shared_ptr<framebuffer_layout> fb_layout_;

public:
	render_pass(device& device_, destruction_queue_manager* dqm, std::shared_ptr<subpass_graph> subpasses,
				std::shared_ptr<framebuffer_layout> fb_layout,
				vk::ArrayProxy<attachment_access> attachment_access_modes);
	~render_pass();

	const std::shared_ptr<framebuffer_layout>& fb_layout() const {
		return fb_layout_;
	}

	const queued_handle<vk::UniqueRenderPass>& native_render_pass() const {
		return native_render_pass_;
	}

	const std::shared_ptr<subpass_graph>& subpasses() const {
		return subpasses_;
	}

	void begin(framebuffer_frame& fb, vk::ArrayProxy<VkClearValue> clear_values,
			   vk::SubpassContents subpass_contents) const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_RENDER_PASS_HPP_ */
