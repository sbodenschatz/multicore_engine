/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_FRAMEBUFFER_HPP_
#define GRAPHICS_FRAMEBUFFER_HPP_

#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/image.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class framebuffer;

class framebuffer_frame {
	uint32_t swapchain_image_index_;
	queued_handle<vk::UniqueFramebuffer> native_framebuffer_;
	framebuffer* owner_;

	framebuffer_frame(uint32_t swapchain_image_index, queued_handle<vk::UniqueFramebuffer> native_framebuffer,
					  framebuffer& owner_)
			: swapchain_image_index_{swapchain_image_index},
			  native_framebuffer_{std::move(native_framebuffer)}, owner_{&owner_} {}

public:
	vk::Framebuffer native_framebuffer() const {
		return native_framebuffer_.get();
	}

	uint32_t swapchain_image_index() const {
		return swapchain_image_index_;
	}

	const framebuffer& owner() const {
		return *owner_;
	}
	framebuffer& owner() {
		return *owner_;
	}
};

class framebuffer {
private:
	device* dev_;
	window* win_;
	glm::uvec2 size_;
	std::vector<image_var> additional_attachments_;
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
		void operator()(T&) const {}
	};

	class view_visitor : public boost::static_visitor<vk::ImageView> {
	public:
		template <typename T>
		vk::ImageView operator()(T& iv) const {
			// TODO: Implement
			static_cast<void>(iv);
			return vk::ImageView();
		}
	};

public:
	framebuffer(device& dev, window& win, device_memory_manager_interface& mem_mgr,
				destruction_queue_manager* destruction_manager, std::shared_ptr<framebuffer_config> config,
				vk::RenderPass compatible_pass);
	~framebuffer();

	const std::shared_ptr<framebuffer_config>& config() const {
		return config_;
	}

	const glm::uvec2& size() const {
		return size_;
	}

	const std::vector<framebuffer_frame>& frames() const {
		return frames_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_FRAMEBUFFER_HPP_ */
