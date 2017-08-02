/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/sampler.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/sampler.hpp>

namespace mce {
namespace graphics {

sampler::sampler(vk::Filter mag_filter, vk::Filter min_filter, vk::SamplerMipmapMode mipmap_mode,
				 addressing_mode address_mode, float mip_lod_bias, boost::optional<float> max_anisotropy,
				 boost::optional<vk::CompareOp> compare_op, float min_lod, float max_lod,
				 vk::BorderColor border_color, bool unnormalized_coordinates)
		: mag_filter_{mag_filter}, min_filter_{min_filter}, mipmap_mode_{mipmap_mode},
		  address_mode_{address_mode}, mip_lod_bias_{mip_lod_bias}, max_anisotropy_{max_anisotropy},
		  compare_op_{compare_op}, min_lod_{min_lod}, max_lod_{max_lod}, border_color_{border_color},
		  unnormalized_coordinates_{unnormalized_coordinates} {
	// TODO: Implement
}

sampler::~sampler() {}

} /* namespace graphics */
} /* namespace mce */
