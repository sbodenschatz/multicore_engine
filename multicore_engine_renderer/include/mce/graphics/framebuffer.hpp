/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_FRAMEBUFFER_HPP_
#define GRAPHICS_FRAMEBUFFER_HPP_

/**
 * \file
 * Provides the encapsulation classes for framebuffers.
 */

#include <cassert>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/image.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class framebuffer;

/// Encapsulates the data specific for a frame / swapchain image in a framebuffer.
/**
 * This class corresponds with the vulkan concept of a framebuffer (vk::Framebuffer) while the
 * graphics::framebuffer class also holds the additional attachment images.
 */
class framebuffer_frame {
	uint32_t swapchain_image_index_;
	queued_handle<vk::UniqueFramebuffer> native_framebuffer_;
	framebuffer* owner_;
	uint32_t owning_pass_;

	framebuffer_frame(uint32_t swapchain_image_index, queued_handle<vk::UniqueFramebuffer> native_framebuffer,
					  framebuffer& owner_, uint32_t owning_pass)
			: swapchain_image_index_{swapchain_image_index},
			  native_framebuffer_{std::move(native_framebuffer)}, owner_{&owner_}, owning_pass_{owning_pass} {
	}

	friend class framebuffer;
	friend class framebuffer_pass;

public:
	/// Allows access to the native vulkan framebuffer.
	vk::Framebuffer native_framebuffer() const {
		return native_framebuffer_.get();
	}

	/// Returns the index of associated swapchain image.
	uint32_t swapchain_image_index() const {
		return swapchain_image_index_;
	}

	/// Returns a reference to the owner framebuffer object.
	const framebuffer& owner() const {
		return *owner_;
	}

	/// Returns a reference to the owner framebuffer object.
	framebuffer& owner() {
		return *owner_;
	}

	/// Indicates in which pass of the owning framebuffer object this framebuffer_frame is used.
	uint32_t owning_pass() const {
		return owning_pass_;
	}
};

/// \brief Represents the parts of a framebuffer (subset of attachments) used by a compatibility equivalence
/// class of render passes.
/**
 * The abstraction framebuffer class represents a collection of attachments that can be used by multiple
 * render passes. However in vulkan a framebuffer object can only be used with render passes that are
 * compatible with the one they were constructed for. This essentially limits a framebuffer to use with render
 * passes that use the same set of attachments of the framebuffer as the same attachment type.
 * Therefore the framebuffer object contains multiple framebuffer_pass objects. These take a subset of the
 * attachments in the high-level framebuffer and creates a vulkan framebuffer for that subset (or one for each
 * swapchain image if it contains a swapchain image attachment).
 */
class framebuffer_pass {
	std::vector<framebuffer_frame> frames_;

	// cppcheck-suppress passedByValue
	explicit framebuffer_pass(std::vector<framebuffer_frame> frames) : frames_{std::move(frames)} {}

	friend class framebuffer;

public:
	/// Returns the associated framebuffer_frame objects.
	const std::vector<framebuffer_frame>& frames() const {
		return frames_;
	}

	/// Provides access to the framebuffer_frame with the given index.
	framebuffer_frame& frame(uint32_t index) {
		return frames_[index];
	}
};

/// \brief Encapsulates the management of a framebuffer consisting of (optionally) a number of swapchain
/// images, a number of additional attachment images and framebuffer_pass objects representing the parts of
/// the buffer used for a specific render_pass.
class framebuffer {
private:
	device* dev_;
	window* win_;
	glm::uvec2 size_{0, 0};
	std::vector<image_var> attachments_;
	std::vector<image_view_var> attachment_views_;
	std::shared_ptr<const framebuffer_config> config_;
	std::vector<framebuffer_pass> passes_;

	class imgview_visitor : public boost::static_visitor<> {
		framebuffer* fb;

	public:
		explicit imgview_visitor(framebuffer* fb) : fb{fb} {}
		void operator()(image_2d& img) const {
			fb->attachment_views_.emplace_back(img.create_view());
		}
		void operator()(image_2d_ds& img) const {
			fb->attachment_views_.emplace_back(img.create_view());
		}
		void operator()(image_2d_depth& img) const {
			fb->attachment_views_.emplace_back(img.create_view());
		}
		void operator()(image_2d_stencil& img) const {
			fb->attachment_views_.emplace_back(img.create_view());
		}
		template <typename T>
		void operator()(T&) const {
			assert(false && "Imageviews in the framebuffer should only be 2d non-layered currently.");
		}
	};

	class view_visitor : public boost::static_visitor<vk::ImageView> {
	public:
		template <typename T>
		vk::ImageView operator()(T& iv) const {
			return iv.native_view();
		}

		vk::ImageView operator()(vk::ImageView iv) const {
			return iv;
		}
	};

public:
	/// \brief Creates a framebuffer on the given device for the given window (holding the swapchain images
	/// and determining the size) using the given memory and destruction managers with the given configuration
	/// to be compatible with the given render pass.
	framebuffer(device& dev, window& win, device_memory_manager_interface& mem_mgr,
				destruction_queue_manager* destruction_manager,
				std::shared_ptr<const framebuffer_config> config,
				std::vector<vk::RenderPass> compatible_passes);
	/// Destroys the framebuffer and releases the resources to the destruction_manager given on construction.
	~framebuffer();

	framebuffer(const framebuffer&) = delete;
	framebuffer& operator=(const framebuffer&) = delete;

	/// Allows access to the framebuffer_config describing the structure of the framebuffer.
	const std::shared_ptr<const framebuffer_config>& config() const {
		return config_;
	}

	/// Returns the size of the framebuffer.
	const glm::uvec2& size() const {
		return size_;
	}

	/// Returns the attachments of the framebuffer.
	/**
	 * If the framebuffer contains a swaphain image it is represented by a placeholder value vk::Image().
	 */
	const std::vector<image_var>& attachments() const {
		return attachments_;
	}

	/// Return views on the attachments of the framebuffer.
	/**
	 * If the framebuffer contains a swaphain image it is represented by a placeholder value vk::ImageView().
	 */
	const std::vector<image_view_var>& attachment_views() const {
		return attachment_views_;
	}

	/// Allows access to the framebuffer_pass objects in this framebuffer.
	const std::vector<framebuffer_pass>& passes() const {
		return passes_;
	}

	/// Provides access to the framebuffer_pass with the given index.
	framebuffer_pass& pass(uint32_t index) {
		return passes_[index];
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
