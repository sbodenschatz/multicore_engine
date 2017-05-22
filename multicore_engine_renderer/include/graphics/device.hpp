/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/graphics/device.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_DEVICE_HPP_
#define GRAPHICS_DEVICE_HPP_

#include "unique_handle.hpp"
#include <boost/container/flat_set.hpp>
#include <graphics/graphics_defs.hpp>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class window;
class application_instance;

class device {
private:
	static const queue_index_t no_queue_index;
	static const queue_family_index_t no_queue_family_index;
	application_instance& app_instance_;
	window& connected_window_;
	vk::PhysicalDevice physical_device_;
	vk::PhysicalDeviceProperties physical_device_properties_;
	queue_index_t graphics_queue_index_ = no_queue_index;
	queue_index_t transfer_queue_index_ = no_queue_index;
	queue_index_t present_queue_index_ = no_queue_index;
	unique_handle<vk::Device, false> native_device_;
	unique_handle<vk::SwapchainKHR> swapchain_;
	vk::Queue graphics_queue_;
	vk::Queue transfer_queue_;
	vk::Queue present_queue_;
	vk::Format surface_format_;

	queue_family_index_t
	find_queue_family(const std::vector<vk::QueueFamilyProperties>& queue_families,
					  vk::QueueFlags required_flags, bool present_required,
					  boost::container::flat_set<queue_family_index_t> excluded_families =
							  boost::container::flat_set<queue_family_index_t>());
	queue_index_t find_queue(const std::vector<vk::QueueFamilyProperties>& queue_families,
							 vk::QueueFlags required_flags, bool present_required,
							 boost::container::flat_set<queue_family_index_t> excluded_families =
									 boost::container::flat_set<queue_family_index_t>(),
							 boost::container::flat_set<queue_index_t> excluded_queues =
									 boost::container::flat_set<queue_index_t>());

	void find_physical_device();
	void find_queue_indexes();
	void create_device();
	void create_swapchain(const window& win);

public:
	device(application_instance& app_inst, window& win);
	~device();

	const std::pair<uint32_t, uint32_t>& graphics_queue_index() const {
		return graphics_queue_index_;
	}

	const std::pair<uint32_t, uint32_t>& present_queue_index() const {
		return present_queue_index_;
	}

	vk::Format surface_format() const {
		return surface_format_;
	}

	const std::pair<uint32_t, uint32_t>& transfer_queue_index() const {
		return transfer_queue_index_;
	}

	const vk::Device& native_device() const {
		return native_device_.get();
	}

	vk::Queue graphics_queue() const {
		return graphics_queue_;
	}

	vk::Queue present_queue() const {
		return present_queue_;
	}

	const vk::SwapchainKHR& swapchain() const {
		return swapchain_.get();
	}

	vk::Queue transfer_queue() const {
		return transfer_queue_;
	}

	window& connected_window() const {
		return connected_window_;
	}

	vk::PhysicalDevice physical_device() const {
		return physical_device_;
	}

	const vk::PhysicalDeviceProperties& physical_device_properties() const {
		return physical_device_properties_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_DEVICE_HPP_ */
