/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/destruction_queue_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_
#define MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_

/**
 * \file
 * Defines the destruction queue management functionality.
 */

#include <boost/variant.hpp>
#include <functional>
#include <mce/graphics/device_memory_handle.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// \brief Provides a ring buffer of queues (one for each index in the frame ring buffer) that are used to
/// defer deletion of resources until asynchronous command execution has finished.
/**
 * Destruction happens by calling the reset member function of the handle without parameters.
 * The destructions in a ring buffer slot happen in the order in which they were enqueued.
 *
 * The following types are supported for destruction:
 *   - vk::UniqueBuffer
 *   - vk::UniqueBufferView
 *   - vk::UniqueCommandBuffer
 *   - vk::UniqueCommandPool
 *   - vk::UniqueDescriptorPool
 *   - vk::UniqueDescriptorSet
 *   - vk::UniqueEvent
 *   - vk::UniqueFence
 *   - vk::UniqueFramebuffer
 *   - vk::UniqueImage
 *   - vk::UniqueImageView
 *   - vk::UniquePipeline
 *   - vk::UniqueQueryPool
 *   - vk::UniqueRenderPass
 *   - vk::UniqueSampler
 *   - vk::UniqueSemaphore
 *   - vk::UniqueShaderModule
 *   - vk::UniqueSurfaceKHR
 *   - vk::UniqueSwapchainKHR
 *   - mce::graphics::device_memory_handle
 *   - mce::graphics::destruction_queue_manager::executor<std::function<void()>>
 */
class destruction_queue_manager {
public:
	/// Wrapper to allow running callables from the destruction queue.
	/**
	 * F must be callable with signature <code>void()</code>.
	 */
	template <typename F>
	struct executor {
		F executee; ///< The callable to be called.
		/// Runs the wrapped function to hook into the destruction queue.
		void reset() {
			executee();
		}
	};

private:
	using element =
			boost::variant<boost::blank, vk::UniqueBuffer, vk::UniqueBufferView, vk::UniqueCommandBuffer,
						   vk::UniqueCommandPool, vk::UniqueDescriptorPool, vk::UniqueDescriptorSet,
						   vk::UniqueEvent, vk::UniqueFence, vk::UniqueFramebuffer, vk::UniqueImage,
						   vk::UniqueImageView, vk::UniquePipeline, vk::UniqueQueryPool, vk::UniqueRenderPass,
						   vk::UniqueSampler, vk::UniqueSemaphore, vk::UniqueShaderModule,
						   vk::UniqueSurfaceKHR, vk::UniqueSwapchainKHR, device_memory_handle,
						   executor<std::function<void()>>>;
	std::mutex queue_mutex;
	device* dev_;
	std::vector<std::vector<element>> queues;
	uint32_t current_ring_index = 0;
	uint32_t ring_slots;

	struct reset_visitor : boost::static_visitor<> {
		void operator()(boost::blank&) {}
		template <typename T>
		void operator()(T& handle) {
			handle.reset();
		}
	};

public:
	/// Creates a destruction queue manager for the given device and with the given number of queues.
	destruction_queue_manager(device* dev, uint32_t ring_slots) : dev_{ dev }, queues{ring_slots}, ring_slots{ ring_slots } {}
	/// \brief Destroys the destruction queue manager and all pending objects after ensuring completion by
	/// waiting for the device to be idle.
	~destruction_queue_manager();

	/// \brief Inserts the resource managed by the given handle to be destroyed when the current ring index is
	/// cleared.
	template <typename T>
	void enqueue(T&& handle) {
		std::lock_guard<std::mutex> lock(queue_mutex);
		queues[current_ring_index].emplace_back(std::move(handle));
	}

	/// Cleans the given ring index and sets it as the new current ring index.
	void cleanup_and_set_current(uint32_t ring_index);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_ */
