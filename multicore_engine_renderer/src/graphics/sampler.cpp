/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/sampler.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/sampler.hpp>

namespace mce {
namespace graphics {

sampler::sampler(const device& dev, destruction_queue_manager* dqm, vk::Filter mag_filter,
				 vk::Filter min_filter, vk::SamplerMipmapMode mipmap_mode,
				 sampler_addressing_mode address_mode, float mip_lod_bias,
				 boost::optional<float> max_anisotropy, boost::optional<vk::CompareOp> compare_op,
				 float min_lod, float max_lod, vk::BorderColor border_color, bool unnormalized_coordinates)
		: mag_filter_{mag_filter}, min_filter_{min_filter}, mipmap_mode_{mipmap_mode},
		  address_mode_{address_mode}, mip_lod_bias_{mip_lod_bias}, max_anisotropy_{max_anisotropy},
		  compare_op_{compare_op}, min_lod_{min_lod}, max_lod_{max_lod}, border_color_{border_color},
		  unnormalized_coordinates_{unnormalized_coordinates} {
	vk::SamplerCreateInfo ci;
	ci.magFilter = mag_filter_;
	ci.minFilter = min_filter_;
	ci.mipmapMode = mipmap_mode_;
	ci.addressModeU = address_mode_.u();
	ci.addressModeV = address_mode_.v();
	ci.addressModeW = address_mode_.w();
	ci.mipLodBias = mip_lod_bias_;
	if(max_anisotropy_) {
		ci.anisotropyEnable = true;
		ci.maxAnisotropy = max_anisotropy_.get();
	} else {
		ci.maxAnisotropy = 1.0f;
		ci.anisotropyEnable = false;
	}
	if(compare_op_) {
		ci.compareEnable = true;
		ci.compareOp = compare_op_.get();
	} else {
		ci.compareEnable = false;
	}
	ci.minLod = min_lod_;
	ci.maxLod = max_lod_;
	ci.borderColor = border_color_;
	ci.unnormalizedCoordinates = unnormalized_coordinates_;
	native_sampler_ = queued_handle<vk::UniqueSampler>(dev->createSamplerUnique(ci), dqm);
}

sampler::~sampler() {}

} /* namespace graphics */
} /* namespace mce */
