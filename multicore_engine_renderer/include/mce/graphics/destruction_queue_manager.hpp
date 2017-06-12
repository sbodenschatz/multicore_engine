/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/destruction_queue.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_
#define MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_

#include <boost/variant.hpp>
#include <functional>
#include <mce/graphics/device_memory_handle.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class destruction_queue_manager {
public:
	template <typename F>
	struct executor {
		F f;
		void reset() {
			f();
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
	device& dev_;
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
	destruction_queue_manager(device& dev, uint32_t ring_slots) : dev_{dev}, ring_slots{ring_slots} {}
	~destruction_queue_manager();

	template <typename T>
	void enqueue(T&& handle) {
		std::lock_guard<std::mutex> lock(queue_mutex);
		queues[current_ring_index].emplace_back(std::move(handle));
	}

	void cleanup_and_set_current(uint32_t ring_index);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESTRUCTION_QUEUE_MANAGER_HPP_ */
