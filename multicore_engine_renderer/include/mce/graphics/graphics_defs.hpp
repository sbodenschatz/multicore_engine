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

/// Bundles the addressing modes for the three dimensions of texel space for a sampler.
class sampler_addressing_mode {
	vk::SamplerAddressMode u_;
	vk::SamplerAddressMode v_;
	vk::SamplerAddressMode w_;

public:
	/// Creates an addressing_mode with the given mode for all dimensions.
	explicit sampler_addressing_mode(vk::SamplerAddressMode mode) : u_{mode}, v_{mode}, w_{mode} {}
	/// Creates an addressing_mode with the given modes for each of the dimensions.
	sampler_addressing_mode(vk::SamplerAddressMode u, vk::SamplerAddressMode v, vk::SamplerAddressMode w)
			: u_{u}, v_{v}, w_{w} {}

	/// Returns the sampler address mode for the u dimension.
	vk::SamplerAddressMode u() const {
		return u_;
	}

	/// Returns the sampler address mode for the v dimension.
	vk::SamplerAddressMode v() const {
		return v_;
	}

	/// Returns the sampler address mode for the w dimension.
	vk::SamplerAddressMode w() const {
		return w_;
	}
};

/// Describes a subpass in the subpass_graph.
struct subpass_entry {
	/// Describes the attachments used by this subpass as inputs and their layout.
	std::vector<vk::AttachmentReference> input;
	/// Describes the attachments used by this subpass as color outputs and their layout.
	std::vector<vk::AttachmentReference> color;
	/// \brief Describes the attachments used to resolve multisample attachments of the subpass into and their
	/// layout.
	std::vector<vk::AttachmentReference> resolve;
	/// \brief Describes the optional attachment the is used by the subpass as a depth and/or stencil buffer
	/// and the corresponding layout.
	boost::optional<vk::AttachmentReference> depth_stencil;
	/// \brief Describes the attachments that are not used by this subpass but must have their contents
	/// preseved through the subpass.
	std::vector<uint32_t> preserve;
};

} // namespace graphics
} // namespace mce

#endif /* GRAPHICS_GRAPHICS_DEFS_HPP_ */
