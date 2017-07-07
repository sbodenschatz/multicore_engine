/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_HPP_
#define MCE_GRAPHICS_IMAGE_HPP_

#include <boost/optional.hpp>
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

template <image_dimension img_dim, bool layered>
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

class base_image_view {
protected:
	queued_handle<vk::UniqueImageView> view_;
	~base_image_view() noexcept;
	base_image_view(queued_handle<vk::UniqueImageView> view, uint32_t base_mip_level, uint32_t mip_levels,
					vk::ComponentMapping component_mapping, vk::Format format, uint32_t base_layer = 0,
					uint32_t layers = 1);

public:
	base_image_view(base_image_view&& other);
	base_image_view& operator=(base_image_view&& other);
};

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
class image_view;

class any_image_view : public base_image_view {
protected:
	using base_image_view::base_image_view;

public:
	template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
	explicit any_image_view(image_view<img_dim, layered, img_aspect>&& other)
			: base_image_view(std::move(other)) {}
};

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
class image_view : public base_image_view {
protected:
	using base_image_view::base_image_view;

public:
	explicit image_view(any_image_view&& other) : base_image_view(std::move(other)) {}
};

namespace detail {

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
struct image_view_type_mapper {};

template <image_aspect_mode img_aspect>
struct image_view_type_mapper<image_dimension::dim_1d, false, img_aspect> {
	static constexpr vk::ImageType vk_img_type = vk::ImageType::e1D;
	static constexpr vk::ImageViewType vk_view_type = vk::ImageViewType::e1D;
};
template <image_aspect_mode img_aspect>
struct image_view_type_mapper<image_dimension::dim_2d, false, img_aspect> {
	static constexpr vk::ImageType vk_img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType vk_view_type = vk::ImageViewType::e2D;
};
template <image_aspect_mode img_aspect>
struct image_view_type_mapper<image_dimension::dim_3d, false, img_aspect> {
	static constexpr vk::ImageType vk_img_type = vk::ImageType::e3D;
	static constexpr vk::ImageViewType vk_view_type = vk::ImageViewType::e3D;
};
template <image_aspect_mode img_aspect>
struct image_view_type_mapper<image_dimension::dim_cube, false, img_aspect> {
	static constexpr vk::ImageType vk_img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType vk_view_type = vk::ImageViewType::eCube;
};

} // namespace detail

class base_image {
protected:
	image_dimension img_dim_;
	bool layered_;
	image_aspect_mode aspect_mode_;
	device* dev_;
	queued_handle<vk::UniqueImage> img_;
	vk::Format format_;
	vk::Extent3D size_;
	uint32_t layers_;
	uint32_t mip_levels_;
	vk::ImageUsageFlags usage_;
	vk::MemoryPropertyFlags required_flags_;
	bool mutable_format_;
	vk::ImageTiling tiling_;

	~base_image() noexcept;
	base_image(image_dimension img_dim, bool layered, image_aspect_mode aspect_mode,
			   vk::ImageCreateFlags base_create_flags, vk::ImageType img_type, device& dev,
			   device_memory_manager_interface& mem_mgr, destruction_queue_manager* destruction_manager,
			   vk::Format format, vk::Extent3D size, uint32_t layers, uint32_t mip_levels,
			   vk::ImageUsageFlags usage,
			   vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
			   bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			   bool preinitialized_layout = false);

public:
	any_image_view create_view(vk::ImageViewType view_type, uint32_t base_layer = 0,
							   uint32_t layers = VK_REMAINING_ARRAY_LAYERS, uint32_t base_mip_level = 0,
							   uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
							   vk::ComponentMapping component_mapping = {},
							   boost::optional<vk::Format> view_format = {});
};

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
class image {};

template <image_dimension img_dim, image_aspect_mode img_aspect>
class image<img_dim, false, img_aspect> : public base_image {
public:
	using size_type = image_size<img_dim, false>;
	image(device& dev, device_memory_manager_interface& mem_mgr,
		  destruction_queue_manager* destruction_manager, vk::Format format, size_type size,
		  uint32_t mip_levels, vk::ImageUsageFlags usage,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  bool preinitialized_layout = false)
			: base_image(img_dim, false, img_aspect, {},
						 detail::image_view_type_mapper<img_dim, false, img_aspect>::vk_img_type, dev,
						 mem_mgr, destruction_manager, format, {size.width, size.height, size.depth},
						 size.layers, mip_levels, usage, required_flags, mutable_format, tiling,
						 preinitialized_layout) {}

	image_view<img_dim, false, img_aspect> create_view(uint32_t base_mip_level = 0,
													   uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
													   vk::ComponentMapping component_mapping = {},
													   boost::optional<vk::Format> view_format = {}) {
		return image_view<img_dim, false, img_aspect>(base_image::create_view(
				detail::image_view_type_mapper<img_dim, false, img_aspect>::vk_view_type, 0, 1,
				base_mip_level, mip_levels, component_mapping, view_format));
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_IMAGE_HPP_ */
