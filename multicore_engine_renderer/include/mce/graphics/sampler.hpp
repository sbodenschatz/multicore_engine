/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/sampler.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SAMPLER_HPP_
#define MCE_GRAPHICS_SAMPLER_HPP_

/**
 * \file
 * Defines the wrapper class for vulkan samplers.
 */

#include <boost/optional.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/graphics_defs.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

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

/// Encapsulates a vulkan sampler object and the associated data.
class sampler {
	vk::Filter mag_filter_;
	vk::Filter min_filter_;
	vk::SamplerMipmapMode mipmap_mode_;
	sampler_addressing_mode address_mode_;
	float mip_lod_bias_;
	boost::optional<float> max_anisotropy_;
	boost::optional<vk::CompareOp> compare_op_;
	float min_lod_;
	float max_lod_;
	vk::BorderColor border_color_;
	bool unnormalized_coordinates_;
	queued_handle<vk::UniqueSampler> native_sampler_;

public:
	/// \brief Creates a sampler on the given device with the given parameters using the given
	/// destruction_queue_manager for resource disposal.
	sampler(const device& dev, destruction_queue_manager* dqm, vk::Filter mag_filter, vk::Filter min_filter,
			vk::SamplerMipmapMode mipmap_mode, sampler_addressing_mode address_mode, float mip_lod_bias,
			boost::optional<float> max_anisotropy, boost::optional<vk::CompareOp> compare_op, float min_lod,
			float max_lod, vk::BorderColor border_color, bool unnormalized_coordinates);
	/// \brief Destroys the sampler wrapper and releases the underlying resources to the
	/// destruction_queue_manager given at construction.
	~sampler();

	/// Returns the used address mode.
	const sampler_addressing_mode& address_mode() const {
		return address_mode_;
	}

	/// Returns the used border_color.
	vk::BorderColor border_color() const {
		return border_color_;
	}

	/// Returns the used compare operation.
	const boost::optional<vk::CompareOp>& compare_op() const {
		return compare_op_;
	}

	/// Returns the used magnification filter.
	vk::Filter mag_filter() const {
		return mag_filter_;
	}

	/// Returns the used maximum anisotropy setting.
	const boost::optional<float>& max_anisotropy() const {
		return max_anisotropy_;
	}

	/// Returns the max level of detail against which the sampler clamps.
	float max_lod() const {
		return max_lod_;
	}

	/// Returns the used minification filter.
	vk::Filter min_filter() const {
		return min_filter_;
	}

	/// Returns the min level of detail against which the sampler clamps.
	float min_lod() const {
		return min_lod_;
	}

	/// Returns the used level of detail bias.
	float mip_lod_bias() const {
		return mip_lod_bias_;
	}

	/// Returns the used mipmap mode.
	vk::SamplerMipmapMode mipmap_mode() const {
		return mipmap_mode_;
	}

	/// Allows access to the underlying vulkan sampler.
	vk::Sampler native_sampler() const {
		return native_sampler_.get();
	}

	/// Returns a bool indicating if the sampler uses unnormalized coordinates.
	bool unnormalized_coordinates() const {
		return unnormalized_coordinates_;
	}
};

} /* namespace graphics */
} /*x namespace mce */

#endif /* MCE_GRAPHICS_SAMPLER_HPP_ */
