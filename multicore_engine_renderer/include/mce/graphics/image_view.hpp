/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image_view.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_VIEW_HPP_
#define MCE_GRAPHICS_IMAGE_VIEW_HPP_

#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

enum class image_view_dimension { dim_1d, dim_2d, dim_3d, dim_cube };

template <image_view_dimension dimension, bool layered = false>
class image_view {
	vk::UniqueImageView native_view_;
	uint32_t base_mip_level_;
	uint32_t mip_levels_;
	vk::ComponentMapping component_mapping_;
	vk::Format format_;
	uint32_t base_layer_;
	uint32_t layers_;

	image_view(vk::UniqueImageView native_view, uint32_t base_mip_level, uint32_t mip_levels,
			   vk::ComponentMapping component_mapping, vk::Format format, uint32_t base_layer = 0,
			   uint32_t layers = 1)
			: native_view_{std::move(native_view)}, base_mip_level_{base_mip_level}, mip_levels_{mip_levels},
			  component_mapping_{component_mapping}, format_{format}, base_layer_{base_layer},
			  layers_{layers} {}

public:
	image_view(image_view&& other) = default;
	image_view& operator=(image_view&& other) = default;
	~image_view() {
		// TODO: Insert resources into deletion manager.
	}

	uint32_t base_layer() const {
		return base_layer_;
	}

	uint32_t base_mip_level() const {
		return base_mip_level_;
	}

	const vk::ComponentMapping& component_mapping() const {
		return component_mapping_;
	}

	vk::Format format() const {
		return format_;
	}

	uint32_t layers() const {
		return layers_;
	}

	uint32_t mip_levels() const {
		return mip_levels_;
	}

	const vk::ImageView& native_view() const {
		return *native_view_;
	}

	template <typename, typename>
	friend class single_image;
	template <typename, typename>
	friend class layered_image;
	friend class image_cube;
	friend class image_cube_layered;
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_IMAGE_VIEW_HPP_ */
