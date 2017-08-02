/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/sampler.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SAMPLER_HPP_
#define MCE_GRAPHICS_SAMPLER_HPP_

#include <boost/optional.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

class sampler {
public:
	class addressing_mode {
		vk::SamplerAddressMode u_;
		vk::SamplerAddressMode v_;
		vk::SamplerAddressMode w_;

	public:
		explicit addressing_mode(vk::SamplerAddressMode mode) : u_{mode}, v_{mode}, w_{mode} {}
		addressing_mode(vk::SamplerAddressMode u, vk::SamplerAddressMode v, vk::SamplerAddressMode w)
				: u_{u}, v_{v}, w_{w} {}

		vk::SamplerAddressMode u() const {
			return u_;
		}

		vk::SamplerAddressMode v() const {
			return v_;
		}

		vk::SamplerAddressMode w() const {
			return w_;
		}
	};

private:
	vk::Filter mag_filter_;
	vk::Filter min_filter_;
	vk::SamplerMipmapMode mipmap_mode_;
	addressing_mode address_mode_;
	float mip_lod_bias_;
	boost::optional<float> max_anisotropy_;
	boost::optional<vk::CompareOp> compare_op_;
	float min_lod_;
	float max_lod_;
	vk::BorderColor border_color_;
	bool unnormalized_coordinates_;
	queued_handle<vk::UniqueSampler> native_sampler_;

public:
	sampler(const device& dev, destruction_queue_manager* dqm, vk::Filter mag_filter, vk::Filter min_filter,
			vk::SamplerMipmapMode mipmap_mode, addressing_mode address_mode, float mip_lod_bias,
			boost::optional<float> max_anisotropy, boost::optional<vk::CompareOp> compare_op, float min_lod,
			float max_lod, vk::BorderColor border_color, bool unnormalized_coordinates);
	~sampler();

	const addressing_mode& address_mode() const {
		return address_mode_;
	}

	vk::BorderColor border_color() const {
		return border_color_;
	}

	const boost::optional<vk::CompareOp>& compare_op() const {
		return compare_op_;
	}

	vk::Filter mag_filter() const {
		return mag_filter_;
	}

	const boost::optional<float>& max_anisotropy() const {
		return max_anisotropy_;
	}

	float max_lod() const {
		return max_lod_;
	}

	vk::Filter min_filter() const {
		return min_filter_;
	}

	float min_lod() const {
		return min_lod_;
	}

	float mip_lod_bias() const {
		return mip_lod_bias_;
	}

	vk::SamplerMipmapMode mipmap_mode() const {
		return mipmap_mode_;
	}

	vk::Sampler native_sampler() const {
		return native_sampler_.get();
	}

	bool unnormalized_coordinates() const {
		return unnormalized_coordinates_;
	}
};

} /* namespace graphics */
} /*x namespace mce */

#endif /* MCE_GRAPHICS_SAMPLER_HPP_ */
