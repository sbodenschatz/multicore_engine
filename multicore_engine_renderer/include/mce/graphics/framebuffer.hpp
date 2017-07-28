/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_FRAMEBUFFER_HPP_
#define GRAPHICS_FRAMEBUFFER_HPP_

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

	framebuffer_frame(uint32_t swapchain_image_index, queued_handle<vk::UniqueFramebuffer> native_framebuffer,
					  framebuffer& owner_)
			: swapchain_image_index_{swapchain_image_index},
			  native_framebuffer_{std::move(native_framebuffer)}, owner_{&owner_} {}

	friend class framebuffer;

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
};

/// \brief Encapsulates the management of a framebuffer consisting of (optionally) a number of swapchain
/// images, a number of additional attachment images and framebuffer_frames holding the native vulkan
/// framebuffers.
class framebuffer {
private:
	device* dev_;
	window* win_;
	glm::uvec2 size_;
	std::vector<image_var> attachments_;
	std::vector<image_view_var> attachment_views_;
	std::shared_ptr<framebuffer_config> config_;
	std::vector<framebuffer_frame> frames_;

	class imgview_visitor : public boost::static_visitor<> {
		framebuffer* fb;

	public:
		imgview_visitor(framebuffer* fb) : fb{fb} {}
		void operator()(image_2d& img) const {
			fb->attachment_views_.emplace_back(img.create_view());
		}
		void operator()(image_2d_ds& img) const {
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
				destruction_queue_manager* destruction_manager, std::shared_ptr<framebuffer_config> config,
				vk::RenderPass compatible_pass);
	/// Destroys the framebuffer and releases the resources to the destruction_manager given on construction.
	~framebuffer();

	/// Allows access to the framebuffer_config describing the structure of the framebuffer.
	const std::shared_ptr<framebuffer_config>& config() const {
		return config_;
	}

	/// Returns the size of the framebuffer.
	const glm::uvec2& size() const {
		return size_;
	}

	/// Returns the associated framebuffer_frame objects.
	const std::vector<framebuffer_frame>& frames() const {
		return frames_;
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
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
