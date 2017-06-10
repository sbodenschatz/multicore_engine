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

	image_view(vk::UniqueImageView native_view) : native_view_{std::move(native_view)} {}

public:
	~image_view() {
		// TODO: Insert resources into deletion manager.
	}

	template <typename, typename>
	friend class image;
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_IMAGE_VIEW_HPP_ */
