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
 * The queues in the ring buffer can be used in order by switching between them using advance() or can be used
 * in the order of explicitly specified indices using cleanup_and_set_current(). However in the latter case
 * the handle destruction order on destruction of the manager after letting the device complete work on all
 * pending objects deviates from the usage order because it always takes place in ring buffer order.
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

	/// Creates an executor from the given function object.
	static executor<std::function<void()>> make_executor(std::function<void()> f) {
		return executor<std::function<void()>>{std::move(f)};
	}

private:
	class element {
		struct reset_visitor : boost::static_visitor<> {
			void operator()(boost::blank&) {}
			template <typename T>
			void operator()(T& handle) {
				handle.reset();
			}
		};

	public:
		boost::variant<boost::blank, vk::UniqueBuffer, vk::UniqueBufferView, vk::UniqueCommandBuffer,
					   vk::UniqueCommandPool, vk::UniqueDescriptorPool, vk::UniqueDescriptorSet,
					   vk::UniqueEvent, vk::UniqueFence, vk::UniqueFramebuffer, vk::UniqueImage,
					   vk::UniqueImageView, vk::UniquePipeline, vk::UniqueQueryPool, vk::UniqueRenderPass,
					   vk::UniqueSampler, vk::UniqueSemaphore, vk::UniqueShaderModule, vk::UniqueSurfaceKHR,
					   vk::UniqueSwapchainKHR, device_memory_handle, executor<std::function<void()>>> data;
		template <typename T>
		element(T&& data)
				: data{std::move(data)} {}
		element(element&& other) noexcept {
			try {
				data = std::move(other.data);
			} catch(...) {
				data = boost::blank{};
			}
		}
		element& operator=(element&& other) noexcept {
			try {
				data = std::move(other.data);
			} catch(...) {
				data = boost::blank{};
			}
			return *this;
		}
		void reset() {
			reset_visitor v;
			data.apply_visitor(v);
		}
	};
	std::mutex queue_mutex;
	device* dev_;
	std::vector<std::vector<element>> queues;
	uint32_t current_ring_index = 0;
	uint32_t ring_slots;

public:
	/// Creates a destruction queue manager for the given device and with the given number of queues.
	destruction_queue_manager(device* dev, uint32_t ring_slots)
			: dev_{dev}, queues{ring_slots}, ring_slots{ring_slots} {}
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

	/// Advances the manager to the next queue and cleans it.
	void advance() {
		cleanup_and_set_current((current_ring_index + 1) % ring_slots);
	}

	/// Cleans the given ring index and sets it as the new current ring index.
	void cleanup_and_set_current(uint32_t ring_index);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_ */
