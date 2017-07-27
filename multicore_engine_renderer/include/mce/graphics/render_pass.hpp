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

/// Describes how a framebuffer attachment is accessed by a render_pass.
struct attachment_access {
	/// The layout the render_pass will expect the attachment to be in when it begins.
	vk::ImageLayout initial_layout = vk::ImageLayout::eUndefined;
	/// The layout the render_pass will leave the attachment in when it is finished.
	vk::ImageLayout final_layout = vk::ImageLayout::ePresentSrcKHR;
	/// The operation / behavior used to load data from this attachment.
	vk::AttachmentLoadOp load_op = vk::AttachmentLoadOp::eDontCare;
	/// The operation / behavior used to store data to this attachment.
	vk::AttachmentStoreOp store_op = vk::AttachmentStoreOp::eDontCare;
	/// The operation / behavior used to load data from the stencil data of this attachment.
	vk::AttachmentLoadOp stencil_load_op = vk::AttachmentLoadOp::eDontCare;
	/// The operation / behavior used to store data to the stencil data of this attachment.
	vk::AttachmentStoreOp stencil_store_op = vk::AttachmentStoreOp::eDontCare;
};

/// Describes a subpass in the subpass_graph.
struct subpass_entry {
	/// Describes the attachments used by this subpass as inputs and their layout.
	std::vector<vk::AttachmentReference> input;
	/// Describes the attachments used by this subpass as color outputs and their layout.
	std::vector<vk::AttachmentReference> color;
	/// \brief Describes the attachments used to resolve multisample attachments of the subpass into and their
	/// layout.
	std::vector<vk::AttachmentReference> resolve;
	/// \brief Describes the optional attachment the is used by the subpass as a depth and/or stencil buffer
	/// and the corresponding layout.
	boost::optional<vk::AttachmentReference> depth_stencil;
	/// \brief Describes the attachments that are not used by this subpass but must have their contents
	/// preseved through the subpass.
	std::vector<uint32_t> preserve;
};

/// Describes the structure of the subpasses for a render_pass.
class subpass_graph {
	std::vector<subpass_entry> subpasses_;
	std::vector<vk::SubpassDependency> dependencies_;

public:
	/// Creates a subpass_graph consisting of the given nodes (subpasses) and edges (dependencies).
	subpass_graph(std::vector<subpass_entry> subpasses, std::vector<vk::SubpassDependency> dependencies)
			: subpasses_{std::move(subpasses)}, dependencies_{std::move(dependencies)} {}

	/// Allows access to the dependencies between the subpasses.
	const std::vector<vk::SubpassDependency>& dependencies() const {
		return dependencies_;
	}

	/// Allows access to the subpasses.
	const std::vector<subpass_entry>& subpasses() const {
		return subpasses_;
	}
};

/// Abstracts a vulkan render pass consisting of multiple rendering steps (subpasses).
/**
 * The subpasses in a render pass work on a common set of (framebuffer) attachment images.
 * Attachment data are handed through the subpass chain per pixel.
 * This means a subsequent subpasss can't do gather operations on the output of a previous pass but it can
 * read the data from the previous pass that corresponds to each pixel.
 * Subpasses can also express pipeline-barrier-like dependencies and perform image layout transitions.
 * See vulkan spec for details.
 *
 * The structure of the subpasses is described in a subpass_graph object.
 * The structure of the used attachments is described using a framebuffer_layout object that can also be used
 * to create an appropriate framebuffer.
 */
class render_pass {
private:
	device& device_;
	queued_handle<vk::UniqueRenderPass> native_render_pass_;
	std::shared_ptr<subpass_graph> subpasses_;
	std::shared_ptr<framebuffer_layout> fb_layout_;

public:
	/// \brief Creates a render_pass on the given device with the given subpass and framebuffer structure and
	/// using the given destruction_queue_manager.
	/**
	 * The created object participates in ownership of the given subpass_graph and framebuffer_layout.
	 */
	render_pass(device& device_, destruction_queue_manager* dqm, std::shared_ptr<subpass_graph> subpasses,
				std::shared_ptr<framebuffer_layout> fb_layout,
				vk::ArrayProxy<attachment_access> attachment_access_modes);
	/// Destroys the render_pass and releases the wrapped native render_pass to the destruction_queue_manager.
	~render_pass();

	/// Allows access to used framebuffer_layout.
	const std::shared_ptr<framebuffer_layout>& fb_layout() const {
		return fb_layout_;
	}

	/// Allows access to the wrapped native vulkan framebuffer.
	vk::RenderPass native_render_pass() const {
		return native_render_pass_.get();
	}

	/// Allows access to the used subpass_graph.
	const std::shared_ptr<subpass_graph>& subpasses() const {
		return subpasses_;
	}

	/// \brief Begins the render_pass in the given command buffer using the given framebuffer_frame, clear
	/// values and mode for the subpass contents.
	void begin(vk::CommandBuffer cb, framebuffer_frame& fb, vk::ArrayProxy<vk::ClearValue> clear_values,
			   vk::SubpassContents subpass_contents) const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_RENDER_PASS_HPP_ */
