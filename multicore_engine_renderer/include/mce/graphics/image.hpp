/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_HPP_
#define MCE_GRAPHICS_IMAGE_HPP_

#include <glm/glm.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <mce/util/math_tools.hpp>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

/// Defines the aspect modes for an image, specifying which aspect(s) it consists of.
enum class image_aspect_mode { color, depth, stencil, depth_stencil };

enum class image_dimension { dim_1d, dim_2d, dim_3d, dim_cube };

class base_image {
protected:
	image_dimension img_type_;
	bool layered_;
	image_aspect_mode aspect_mode_;
	device* dev_;
	destruction_queue_manager* destruction_manager_;
	vk::Format format_;
	vk::Extent3D size_;
	uint32_t layers_;
	uint32_t mip_levels_;
	vk::ImageUsageFlags usage_;
	vk::MemoryPropertyFlags required_flags_;
	bool mutable_format_;
	vk::ImageTiling tiling_;

	~base_image() noexcept;
	base_image(image_dimension img_type, bool layered, image_aspect_mode aspect_mode, device& dev,
			   device_memory_manager_interface& mem_mgr, destruction_queue_manager* destruction_manager,
			   vk::Format format, vk::Extent3D size, uint32_t layers, uint32_t mip_levels,
			   vk::ImageUsageFlags usage,
			   vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
			   bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			   bool preinitialized_layout = false);

public:
};

template <image_dimension img_type, bool layered>
struct image_size {};

template <>
struct image_size<image_dimension::dim_1d, false> {
	uint32_t width;
	uint32_t height = 1;
	uint32_t depth = 1;
	uint32_t layers = 1;
	// cppcheck-suppress noExplicitConstructor
	image_size(uint32_t width) : width{width} {}
};
template <>
struct image_size<image_dimension::dim_2d, false> {
	uint32_t width;
	uint32_t height;
	uint32_t depth = 1;
	uint32_t layers = 1;
	image_size(uint32_t width, uint32_t height) : width{width}, height{height} {}
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec2 size) : width{size.x}, height{size.y} {}
};
template <>
struct image_size<image_dimension::dim_3d, false> {
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t layers = 1;
	image_size(uint32_t width, uint32_t height, uint32_t depth)
			: width{width}, height{height}, depth{depth} {}
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec3 size) : width{size.x}, height{size.y}, depth{size.z} {}
};
template <>
struct image_size<image_dimension::dim_cube, false> {
	uint32_t width;
	uint32_t height;
	uint32_t depth = 1;
	uint32_t layers = 6;
	image_size(uint32_t width, uint32_t height) : width{width}, height{height} {}
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec2 size) : width{size.x}, height{size.y} {}
};

template <image_dimension img_type, bool layered, image_aspect_mode img_aspect>
class image {};

template <image_dimension img_type, image_aspect_mode img_aspect>
class image<img_type, false, img_aspect> : public base_image {
public:
	using size_type = image_size<img_type, false>;
	image(device& dev, device_memory_manager_interface& mem_mgr,
		  destruction_queue_manager* destruction_manager, vk::Format format, size_type size,
		  uint32_t mip_levels, vk::ImageUsageFlags usage,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  bool preinitialized_layout = false)
			: base_image(img_type, false, img_aspect, dev, mem_mgr, destruction_manager, format,
						 {size.x, size.y, size.z}, size.layers, mip_levels, usage, required_flags,
						 mutable_format, tiling, preinitialized_layout) {}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_IMAGE_HPP_ */
