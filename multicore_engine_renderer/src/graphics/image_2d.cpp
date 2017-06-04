/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/image_2d.hpp>

namespace mce {
namespace graphics {

image::image(vk::Format format, glm::uvec2 size, vk::ImageUsageFlags usage, vk::ImageLayout layout,
			 bool mutable_format, vk::ImageTiling tiling, uint32_t mip_levels, uint32_t layers)
		: format_{format}, size_{size}, usage_{usage}, layout_{layout}, mutable_format_{mutable_format},
		  tiling_{tiling}, mip_levels_{mip_levels}, layers_{layers} {}

} /* namespace graphics */
} /* namespace mce */
