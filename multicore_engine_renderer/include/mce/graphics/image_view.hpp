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

class tag_1d {};
class tag_2d {};
class tag_3d {};
class tag_cube {};

template <typename Tag, bool layered = false>
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
