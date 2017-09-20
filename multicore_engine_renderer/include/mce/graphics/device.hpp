/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/device.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_DEVICE_HPP_
#define GRAPHICS_DEVICE_HPP_

/**
 * \file
 * Provides the definition of the device and queue handling class.
 */

#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class window;
class instance;

/// Manages the vulkan device including associated queues.
class device {
private:
	std::vector<vk::PhysicalDeviceType> device_type_preferences_;
	std::vector<std::string> device_preferences_;
	static const queue_index_t no_queue_index;
	static const queue_family_index_t no_queue_family_index;
	instance& instance_;
	vk::PhysicalDevice physical_device_;
	vk::PhysicalDeviceProperties physical_device_properties_;
	queue_index_t graphics_queue_index_ = no_queue_index;
	queue_index_t transfer_queue_index_ = no_queue_index;
	queue_index_t present_queue_index_ = no_queue_index;
	vk::UniqueDevice native_device_;
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
	/// Defines the type of use for a format is checked in a supported format query
	enum class format_support_query_type {
		/// Query support of the format for linearly tiled images.
		linear_tiling_image,
		/// Query support of the format for optimally tiled images.
		optimal_tiling_image,
		/// Query support of the format for buffers.
		buffer
	};

	/// Creates a device object from the given instance.
	explicit device(instance& app_inst,
					std::vector<vk::PhysicalDeviceType> device_type_preferences =
							{{vk::PhysicalDeviceType::eDiscreteGpu, vk::PhysicalDeviceType::eIntegratedGpu}},
					std::vector<std::string> device_preferences = {});
	/// Releases the resources associated with the device object.
	~device();

	/// \brief Returns the first format in candidates that supports the given required feature flags for the
	/// given format_support_query_type.
	/**
	 * If none of them supports the required flags an empty optional is returned.
	 */
	boost::optional<vk::Format> best_supported_format_try(
			vk::ArrayProxy<const vk::Format> candidates, vk::FormatFeatureFlags required_flags = {},
			format_support_query_type query_type = format_support_query_type::optimal_tiling_image) const
			noexcept;

	/// \brief Returns the first format in candidates that supports the given required feature flags for the
	/// given format_support_query_type.
	/**
	 * If none of them supports the required flags an exception of type mce::graphics_exception is thrown.
	 */
	vk::Format best_supported_format(
			vk::ArrayProxy<const vk::Format> candidates, vk::FormatFeatureFlags required_flags = {},
			format_support_query_type query_type = format_support_query_type::optimal_tiling_image) const;

	/// \brief Returns the best depth-only format that supports the given required feature flags (additionally
	/// to eDepthStencilAttachment) for the given format_support_query_type.
	/**
	 * If none of them supports the required flags an exception of type mce::graphics_exception is thrown.
	 */
	vk::Format best_supported_depth_attachment_format(
			vk::FormatFeatureFlags additional_required_flags = {},
			format_support_query_type query_type = format_support_query_type::optimal_tiling_image) const;

	/// \brief Returns the best depth-stencil format that supports the given required feature flags
	/// (additionally to eDepthStencilAttachment) for the given format_support_query_type.
	/**
	 * If none of them supports the required flags an exception of type mce::graphics_exception is thrown.
	 */
	vk::Format best_supported_depth_stencil_attachment_format(
			vk::FormatFeatureFlags additional_required_flags = {},
			format_support_query_type query_type = format_support_query_type::optimal_tiling_image) const;

	/// Allows calling native device member function using the operator -> on the wrapper device.
	const vk::Device* operator->() const {
		return native_device_.operator->();
	}

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
		return *native_device_;
	}

	/// Allows access to the underlying vulkan device.
	vk::Device native_device() {
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
