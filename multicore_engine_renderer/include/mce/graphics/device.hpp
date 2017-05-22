/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_DEVICE_HPP_
#define GRAPHICS_DEVICE_HPP_

/**
 * \file
 * Provides the definition of the device and queue handling class.
 */

#include <boost/container/flat_set.hpp>
#include <mce/graphics/application_instance.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <mce/graphics/unique_handle.hpp>
#include <utility>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class window;
class application_instance;

/// Manages the vulkan device including associated queues.
class device {
private:
	static const queue_index_t no_queue_index;
	static const queue_family_index_t no_queue_family_index;
	application_instance& app_instance_;
	vk::PhysicalDevice physical_device_;
	vk::PhysicalDeviceProperties physical_device_properties_;
	queue_index_t graphics_queue_index_ = no_queue_index;
	queue_index_t transfer_queue_index_ = no_queue_index;
	queue_index_t present_queue_index_ = no_queue_index;
	unique_handle<vk::Device, false> native_device_;
	vk::Queue graphics_queue_;
	vk::Queue transfer_queue_;
	vk::Queue present_queue_;

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

public:
	/// Creates a device object from the given application_instance.
	device(application_instance& app_inst);
	/// Releases the resources associated with the device object.
	~device();

	/// Returns the queue family index and queue index for the graphics queue.
	const queue_index_t& graphics_queue_index() const {
		return graphics_queue_index_;
	}

	/// Returns the queue family index and queue index for the present queue.
	const queue_index_t& present_queue_index() const {
		return present_queue_index_;
	}

	/// Returns the queue family index and queue index for the transfer queue.
	const queue_index_t& transfer_queue_index() const {
		return transfer_queue_index_;
	}

	/// Returns a reference to the underlying vulkan device.
	const vk::Device& native_device() const {
		return native_device_.get();
	}

	/// Returns a handle for the graphics queue.
	vk::Queue graphics_queue() const {
		return graphics_queue_;
	}

	/// Returns a handle for the present queue.
	vk::Queue present_queue() const {
		return present_queue_;
	}
	/// Returns a handle for the transfer queue.
	vk::Queue transfer_queue() const {
		return transfer_queue_;
	}

	/// Returns a handle to the physical device used.
	vk::PhysicalDevice physical_device() const {
		return physical_device_;
	}

	/// Returns a reference to the properties struct for the physical device used.
	const vk::PhysicalDeviceProperties& physical_device_properties() const {
		return physical_device_properties_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_DEVICE_HPP_ */
