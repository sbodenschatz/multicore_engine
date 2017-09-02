/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/graphics_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_
#define MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_

/**
 * \file
 * Defines the graphics_system class.
 */

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

/// Implements a system providing basic graphics resources and functionality.
/**
 * It initializes vulkan resources like the instance and device.
 * It creates a graphics::window for the glfw::window from the window_system.
 *
 * The system also provides general graphics objects like a device_memory_manager, destruction_queue_manager,
 * transfer_manager, texture_manager and graphics_manager and run per-frame hooks on them where needed.
 *
 * The graphics system also handles swapchain image acquiring and presenting and submitting rendering command
 * buffers to the graphics queues after collecting them during a frame.
 */
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
	containers::dynamic_array<vk::UniqueSemaphore> pre_present_semaphores_;
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
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1100;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1100;
	}

	/// \brief Creates the graphics_system taking the window_system as a dependency and optionally allows to
	/// adding extensions and setting the validation level during vulkan initialization.
	/**
	 * Should only be called in core::engine::add_system, but can't be made private and friended with engine
	 * because the construction takes place in std::make_unique.
	 */
	graphics_system(core::engine& eng, core::window_system& win_sys,
					const std::vector<std::string>& extensions = {},
					unsigned int validation_level = instance::default_validation_level);
	/// Destroys the graphics_system and releases the underlying resources.
	virtual ~graphics_system();

	/// Implements the hooked logic that happens at the beginning of the frame.
	/**
	 * Acquires the swapchain image, waits for the frame fence for the previous frame for the same swapchain
	 * image. Then the system updates the transfer_manager and destruction_queue_manager.
	 */
	void prerender(const mce::core::frame_time& frame_time) override;

	/// Implements the hooked logic that happens at the end of the frame.
	/**
	 * Submits the collected command buffers to the graphics queue and presents the swapchain image after they
	 * complete execution. Afterwards ends the frame for the transfer_manager.
	 */
	void postrender(const mce::core::frame_time& frame_time) override;

	/// Allows access to the destruction_queue_manager.
	const graphics::destruction_queue_manager& destruction_queue_manager() const {
		return destruction_queue_manager_;
	}

	/// Allows access to the destruction_queue_manager.
	graphics::destruction_queue_manager& destruction_queue_manager() {
		return destruction_queue_manager_;
	}

	/// Allows access to the vulkan device wrapper.
	const graphics::device& device() const {
		return device_;
	}

	/// Allows access to the vulkan device wrapper.
	graphics::device& device() {
		return device_;
	}

	/// Allows access to the graphics_manager.
	const graphics::graphics_manager& graphics_manager() const {
		return graphics_manager_;
	}

	/// Allows access to the graphics_manager.
	graphics::graphics_manager& graphics_manager() {
		return graphics_manager_;
	}

	/// Allows access to the vulkan instance wrapper.
	const graphics::instance& instance() const {
		return instance_;
	}

	/// Allows access to the vulkan instance wrapper.
	graphics::instance& instance() {
		return instance_;
	}

	/// Allows access to the device_memory manager.
	const graphics::device_memory_manager& memory_manager() const {
		return memory_manager_;
	}

	/// Allows access to the device_memory manager.
	graphics::device_memory_manager& memory_manager() {
		return memory_manager_;
	}

	/// Allows access to the texture_manager.
	const graphics::texture_manager& texture_manager() const {
		return texture_manager_;
	}

	/// Allows access to the texture_manager.
	graphics::texture_manager& texture_manager() {
		return texture_manager_;
	}

	/// Allows access to the transfer_manager.
	const graphics::transfer_manager& transfer_manager() const {
		return transfer_manager_;
	}

	/// Allows access to the transfer_manager.
	graphics::transfer_manager& transfer_manager() {
		return transfer_manager_;
	}

	/// Allows access to the vulkan window wrapping object.
	const graphics::window& window() const {
		return window_;
	}

	/// Allows access to the vulkan window wrapping object.
	graphics::window& window() {
		return window_;
	}

	/// Returns the index of the swapchain image that is currently used for rendering.
	uint32_t current_swapchain_image() const {
		return current_swapchain_image_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_SYSTEM_HPP_ */
