/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_

/**
 * \file
 * Defines the wrapper class for descriptor set layouts.
 */

#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

/// Represents the data for a binding specification in the descriptor set layout.
struct descriptor_set_layout_binding_element {
	uint32_t binding;					///< The index of the specified bindings.
	vk::DescriptorType descriptor_type; ///< The descriptor type of binding specified.
	uint32_t descriptor_count;			///< The number of descriptors in this binding.
	vk::ShaderStageFlags stage_flags;   ///< The pipeline stages to which the binding should be available.
	std::vector<vk::Sampler> immutable_samplers; ///< Optional immutable samplers for sampler bindings.
};

/// Encapsulates a vulkan descriptor set layout and the associated data.
class descriptor_set_layout {
	std::vector<descriptor_set_layout_binding_element> bindings_;
	queued_handle<vk::UniqueDescriptorSetLayout> native_layout_;

public:
	/// \brief Creates a descriptor_set_layout on the given device containing the given bindings using the
	/// given destruction manager.
	descriptor_set_layout(const device& dev, destruction_queue_manager* dqm,
						  std::vector<descriptor_set_layout_binding_element> bindings);
	/// \brief Destroys the descriptor_set_layout wrapper and releases the native resources to the
	/// destruction_queue_manager given at construction.
	~descriptor_set_layout();

	/// Allows access to the binding data contained in this descriptor_set_layout.
	const std::vector<descriptor_set_layout_binding_element>& bindings() const {
		return bindings_;
	}

	/// Allows access to the native vulkan descriptor set layout.
	vk::DescriptorSetLayout native_layout() const {
		return native_layout_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_ */
