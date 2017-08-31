/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/graphics_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_
#define MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_

#include <mce/containers/dynamic_array.hpp>
#include <mce/core/system.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_manager.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <mce/graphics/transfer_manager.hpp>
#include <mce/graphics/window.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace core {
class engine;
class window_system;
} // namespace core
namespace graphics {

class graphics_system : public core::system {
	core::engine& eng;
	graphics::instance instance_;
	graphics::device device_;
	graphics::window window_;
	graphics::device_memory_manager memory_manager_;
	graphics::destruction_queue_manager destruction_queue_manager_;
	graphics::transfer_manager transfer_manager_;
	graphics::texture_manager texture_manager_;
	graphics::graphics_manager graphics_manager_;

	vk::UniqueSemaphore tmp_semaphore_;
	containers::dynamic_array<vk::UniqueSemaphore> acquire_semaphores_;
	containers::dynamic_array<vk::UniqueSemaphore> present_semaphores_;
	containers::dynamic_array<vk::UniqueFence> fences_;

	uint32_t current_swapchain_image_;
	std::vector<queued_handle<vk::UniqueCommandBuffer>> pending_command_buffers_;
	std::vector<vk::CommandBuffer> cmd_buff_handles_;

	command_pool render_queue_cmd_pool_;
	command_pool present_queue_cmd_pool_;
	containers::dynamic_array<vk::UniqueCommandBuffer> render_queue_start_frame_cmd_buffers_;
	containers::dynamic_array<vk::UniqueCommandBuffer> render_queue_end_frame_cmd_buffers_;
	containers::dynamic_array<vk::UniqueCommandBuffer> present_queue_end_frame_cmd_buffers_;

public:
	int pre_phase_ordering() const noexcept override {
		return 0x1100;
	}
	int post_phase_ordering() const noexcept override {
		return 0x1100;
	}

	graphics_system(core::engine& eng, core::window_system& win_sys,
					const std::vector<std::string>& extensions = {},
					unsigned int validation_level = instance::default_validation_level);
	virtual ~graphics_system();

	void prerender(const mce::core::frame_time& frame_time) override;
	void postrender(const mce::core::frame_time& frame_time) override;

	const graphics::destruction_queue_manager& destruction_queue_manager() const {
		return destruction_queue_manager_;
	}

	graphics::destruction_queue_manager& destruction_queue_manager() {
		return destruction_queue_manager_;
	}

	const graphics::device& device() const {
		return device_;
	}

	graphics::device& device() {
		return device_;
	}

	const graphics::graphics_manager& graphics_manager() const {
		return graphics_manager_;
	}

	graphics::graphics_manager& graphics_manager() {
		return graphics_manager_;
	}

	const graphics::instance& instance() const {
		return instance_;
	}

	graphics::instance& instance() {
		return instance_;
	}

	const graphics::device_memory_manager& memory_manager() const {
		return memory_manager_;
	}

	graphics::device_memory_manager& memory_manager() {
		return memory_manager_;
	}

	const graphics::texture_manager& texture_manager() const {
		return texture_manager_;
	}

	graphics::texture_manager& texture_manager() {
		return texture_manager_;
	}

	const graphics::transfer_manager& transfer_manager() const {
		return transfer_manager_;
	}

	graphics::transfer_manager& transfer_manager() {
		return transfer_manager_;
	}

	const graphics::window& window() const {
		return window_;
	}

	graphics::window& window() {
		return window_;
	}

	uint32_t current_swapchain_image() const {
		return current_swapchain_image_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_ */
