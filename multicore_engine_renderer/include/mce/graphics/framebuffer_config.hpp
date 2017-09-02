/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/framebuffer_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_
#define MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_

/**
 * \file
 * Provides the configuration classes for framebuffers.
 */

#include <mce/graphics/image.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Describes the configuration of an attachment in a framebuffer.
class framebuffer_attachment_config {
	vk::Format format_;
	image_aspect_mode aspect_mode_;
	vk::AttachmentDescriptionFlags flags_;
	bool is_swapchain_image_ = false;

	friend class window;

public:
	/// \brief Constructs a framebuffer_attachment_config with the given parameters for a non-swapchain-image
	/// attachment.
	/**
	 * A framebuffer_attachment_config object for a swapchain-image can only be created by the window managing
	 * the swapchain.
	 */
	framebuffer_attachment_config(vk::Format format, image_aspect_mode aspect_mode = image_aspect_mode::color,
								  vk::AttachmentDescriptionFlags flags = {})
			: format_{format}, aspect_mode_{aspect_mode}, flags_{flags} {}

	/// Returns the image_aspect_mode for the attachment.
	/**
	 * This parameter indicates if the image is a color image or a depth and/or stencil image.
	 */
	image_aspect_mode aspect_mode() const {
		return aspect_mode_;
	}

	/// Returns the flags for the attachment that are passed through to vulkan.
	vk::AttachmentDescriptionFlags flags() const {
		return flags_;
	}

	/// Returns the pixel format for the attachment.
	vk::Format format() const {
		return format_;
	}

	/// Returns a boolean indicating if this attachment is a swapchain image.
	bool is_swapchain_image() const {
		return is_swapchain_image_;
	}
};

/// \brief Represents the configuration for the parts of a framebuffer (subset of attachments) used by a
/// compatibility equivalence class of render passes.
/**
 * The abstraction framebuffer class represents a collection of attachments that can be used by multiple
 * render passes. However in vulkan a framebuffer object can only be used with render passes that are
 * compatible with the one they were constructed for. This essentially limits a framebuffer to use with render
 * passes that use the same set of attachments of the framebuffer as the same attachment type.
 * Therefore the framebuffer object contains multiple framebuffer_pass objects. These take a subset of the
 * attachments in the high-level framebuffer and creates a vulkan framebuffer for that subset (or one for each
 * swapchain image if it contains a swapchain image attachment).
 */
class framebuffer_pass_config {
	std::vector<uint32_t> used_attachments_;

public:
	/// Creates a framebuffer_pass_config referencing the given attachments from the framebuffer by index.
	// cppcheck-suppress passedByValue
	explicit framebuffer_pass_config(std::vector<uint32_t> used_attachments)
			: used_attachments_{std::move(used_attachments)} {}

	/// Allows read-only access to the referenced attachments.
	const std::vector<uint32_t>& used_attachments() const {
		return used_attachments_;
	}
};

/// \brief Describes the configuration for a framebuffer (composed of framebuffer_attachment_config and
/// framebuffer_pass_config entries).
class framebuffer_config {
	std::vector<framebuffer_attachment_config> attachment_configs_;
	std::vector<framebuffer_pass_config> passes_;

	friend class window;

public:
	/// \brief Constructs a framebuffer_config from the given framebuffer_attachment_config and
	/// framebuffer_pass_config entries.
	// cppcheck-suppress passedByValue
	explicit framebuffer_config(std::vector<framebuffer_attachment_config> attachment_configs,
								// cppcheck-suppress passedByValue
								std::vector<framebuffer_pass_config> passes)
			: attachment_configs_{std::move(attachment_configs)}, passes_{std::move(passes)} {}

	/// Allows access to the framebuffer_attachment_config entries.
	const std::vector<framebuffer_attachment_config>& attachment_configs() const {
		return attachment_configs_;
	}

	/// Allows access to the framebuffer_pass_config entries.
	const std::vector<framebuffer_pass_config>& passes() const {
		return passes_;
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_FRAMEBUFFER_CONFIG_HPP_ */
