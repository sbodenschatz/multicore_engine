/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/graphics_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_GRAPHICS_DEFS_HPP_
#define GRAPHICS_GRAPHICS_DEFS_HPP_

/**
 * \file
 * Provides general definitions for the graphics subsystem.
 */

#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// \brief Type used to represent the index of a queue of a vulkan device (consisting of the family index and
/// the index with the family).
typedef std::pair<uint32_t, uint32_t> queue_index_t;
/// Type used to represent the index of a queue family of a vulkan device.
typedef uint32_t queue_family_index_t;

/// Represents the data for a binding specification in the descriptor set layout.
struct descriptor_set_layout_binding_element {
	uint32_t binding;					///< The index of the specified bindings.
	vk::DescriptorType descriptor_type; ///< The descriptor type of binding specified.
	uint32_t descriptor_count;			///< The number of descriptors in this binding.
	vk::ShaderStageFlags stage_flags;   ///< The pipeline stages to which the binding should be available.
	std::vector<vk::Sampler> immutable_samplers; ///< Optional immutable samplers for sampler bindings.
};

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_GRAPHICS_DEFS_HPP_ */
