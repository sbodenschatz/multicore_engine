/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_HPP_
#define MCE_GRAPHICS_IMAGE_HPP_

/**
 * \file
 * Defines the classes that manage images and their associated storage and
 * the classes for views on the images.
 */

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

/// Defines the dimensionalities for an image.
enum class image_dimension { dim_1d, dim_2d, dim_3d, dim_cube };

/// \brief Empty unspecialized definition for image_size template class. Specializations are used to
/// selectively convert image sizes from appropriate types.
template <image_dimension img_dim, bool layered>
struct image_size {};

/// Specialization of image_size for unlayered 1d-images.
template <>
struct image_size<image_dimension::dim_1d, false> {
	uint32_t width;		 ///< The width of the image object.
	uint32_t height = 1; ///< The height of the image object.
	uint32_t depth = 1;  ///< The depth of the image object.
	uint32_t layers = 1; ///< The number of layers of the image object.
	/// Allows taking a single unsigned integer as the size.
	// cppcheck-suppress noExplicitConstructor
	image_size(uint32_t width) : width{width} {}
};
/// Specialization of image_size for unlayered 2d-images.
template <>
struct image_size<image_dimension::dim_2d, false> {
	uint32_t width;		 ///< The width of the image object.
	uint32_t height;	 ///< The height of the image object.
	uint32_t depth = 1;  ///< The depth of the image object.
	uint32_t layers = 1; ///< The number of layers of the image object.
	/// Allows taking two unsigned integers as the size.
	image_size(uint32_t width, uint32_t height) : width{width}, height{height} {}
	/// Allows taking a 2d unsigned integer vector as the size.
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec2 size) : width{size.x}, height{size.y} {}
};
/// Specialization of image_size for unlayered 3d-images.
template <>
struct image_size<image_dimension::dim_3d, false> {
	uint32_t width;		 ///< The width of the image object.
	uint32_t height;	 ///< The height of the image object.
	uint32_t depth;		 ///< The depth of the image object.
	uint32_t layers = 1; ///< The number of layers of the image object.
	/// Allows taking three unsigned integers as the size.
	image_size(uint32_t width, uint32_t height, uint32_t depth)
			: width{width}, height{height}, depth{depth} {}
	/// Allows taking a 3d unsigned integer vector as the size.
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec3 size) : width{size.x}, height{size.y}, depth{size.z} {}
};
/// Specialization of image_size for unlayered cubemap-images.
template <>
struct image_size<image_dimension::dim_cube, false> {
	uint32_t width;		 ///< The width of the image object.
	uint32_t height;	 ///< The height of the image object.
	uint32_t depth = 1;  ///< The depth of the image object.
	uint32_t layers = 6; ///< The number of layers of the image object.
	/// Allows taking two unsigned integers as the size.
	image_size(uint32_t width, uint32_t height) : width{width}, height{height} {}
	/// Allows taking a 2d unsigned integer vector as the size.
	// cppcheck-suppress noExplicitConstructor
	image_size(glm::uvec2 size) : width{size.x}, height{size.y} {}
};

class base_image_view {
	queued_handle<vk::UniqueImageView> view_;
	uint32_t base_mip_level_;
	uint32_t mip_levels_;
	vk::ComponentMapping component_mapping_;
	vk::Format format_;
	uint32_t base_layer_;
	uint32_t layers_;

protected:
	~base_image_view() noexcept;
	base_image_view(queued_handle<vk::UniqueImageView> view, uint32_t base_mip_level, uint32_t mip_levels,
					vk::ComponentMapping component_mapping, vk::Format format, uint32_t base_layer = 0,
					uint32_t layers = 1);

public:
	base_image_view(base_image_view&& other) noexcept = default;
	base_image_view& operator=(base_image_view&& other) noexcept = default;
};

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
class image_view;

class any_image_view : public base_image_view {
protected:
	using base_image_view::base_image_view;

	any_image_view(queued_handle<vk::UniqueImageView> view, uint32_t base_mip_level, uint32_t mip_levels,
				   vk::ComponentMapping component_mapping, vk::Format format, uint32_t base_layer = 0,
				   uint32_t layers = 1);

	friend class base_image;

public:
	any_image_view(any_image_view&& other) noexcept = default;
	any_image_view& operator=(any_image_view&& other) noexcept = default;
	template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect>
	explicit any_image_view(image_view<img_dim, layered, img_aspect>&& other)
			: base_image_view(std::move(other)) {}
};

template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect = image_aspect_mode::color>
class image_view : public base_image_view {
protected:
	using base_image_view::base_image_view;

public:
	image_view(image_view<img_dim, layered, img_aspect>&& other) noexcept = default;
	image_view<img_dim, layered, img_aspect>&
	operator=(image_view<img_dim, layered, img_aspect>&& other) noexcept = default;
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

/// Implementation base for all image classes defining the common functionality for images.
/**
 * \warning Does not provide polymorphic destruction.
 */
class base_image {
	image_dimension img_dim_;
	bool layered_;
	image_aspect_mode aspect_mode_;
	vk::ImageType img_type_;
	device* dev_;
	queued_handle<device_memory_handle> mem_handle_;
	queued_handle<vk::UniqueImage> img_;
	vk::Format format_;
	vk::Extent3D size_;
	uint32_t layers_;
	uint32_t mip_levels_;
	vk::ImageUsageFlags usage_;
	vk::MemoryPropertyFlags required_flags_;
	bool mutable_format_;
	vk::ImageTiling tiling_;
	vk::ImageLayout layout_;

protected:
	/// Destroys the image object and releases it associated resources to the deletion queue.
	~base_image() noexcept;
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
	base_image(image_dimension img_dim, bool layered, image_aspect_mode aspect_mode,
			   vk::ImageCreateFlags base_create_flags, vk::ImageType img_type, device& dev,
			   device_memory_manager_interface& mem_mgr, destruction_queue_manager* destruction_manager,
			   vk::Format format, vk::Extent3D size, uint32_t layers, uint32_t mip_levels,
			   vk::ImageUsageFlags usage,
			   vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
			   bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			   bool preinitialized_layout = false);

	/// Returns the aspect flags for the aspect mode of the image.
	/**
	 * For color, depth, and stencil only the respective flag is set.
	 * For depth_stencil both the depth and stencil flag are set.
	 * The returned aspect flags set is the usual value used for views into images of this aspect mode.
	 */
	vk::ImageAspectFlags default_aspect_flags() const;

public:
	/// Calculates and returns the number of mip levels in a full chain for the given image size.
	template <typename T>
	static uint32_t full_mip_levels(T size) {
		return uint32_t(1 + floor(log2(util::component_max(size))));
	}

	/// Creates and returns an image view for the image object using the given view parameters.
	any_image_view create_view(vk::ImageViewType view_type, uint32_t base_layer = 0,
							   uint32_t layers = VK_REMAINING_ARRAY_LAYERS, uint32_t base_mip_level = 0,
							   uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
							   vk::ComponentMapping component_mapping = {},
							   boost::optional<vk::Format> view_format = {});

	/// Returns the format of the image data.
	vk::Format format() const {
		return format_;
	}

	/// Allows access to the handle for the underlying vulkan image object.
	const vk::Image& native_image() const {
		return *img_;
	}

	/// \brief Returns the current layout according to the tracking performed with generate_transition,
	/// mark_layout_undefined, and set_layout_external.
	vk::ImageLayout tracked_layout() const {
		return layout_;
	}

	/// Returns the number of mip levels in the image.
	uint32_t mip_levels() const {
		return mip_levels_;
	}

	/// Returns true if the image was created with mutable format, allowing views with different formats.
	bool mutable_format() const {
		return mutable_format_;
	}

	/// Returns the size of the image.
	vk::Extent3D size() const {
		return size_;
	}

	/// Returns the tiling of the image.
	vk::ImageTiling tiling() const {
		return tiling_;
	}

	/// Returns the allowed usage flags for the image.
	vk::ImageUsageFlags usage() const {
		return usage_;
	}

	/// Returns the parent device.
	const device& dev() const {
		return *dev_;
	}

	/// Generates and returns a layout transition image memory barrier.
	/**
	 * It is assumed that the barriers returned from called to this member function are executed in the same
	 * order as the calls and that none of them are dropped.
	 * The tracked layout is updated immediately.
	 */
	vk::ImageMemoryBarrier generate_transition(vk::ImageLayout new_layout, vk::AccessFlags src_access,
											   vk::AccessFlags dst_access) {
		vk::ImageMemoryBarrier b(
				src_access, dst_access, layout_, new_layout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
				*img_, vk::ImageSubresourceRange(default_aspect_flags(), 0, mip_levels_, 0, layers_));
		layout_ = new_layout;
		return b;
	}

	/// Resets the tracked layout into an undefined state.
	/**
	 * The tracked layout is updated immediately.
	 */
	void mark_layout_undefined() {
		layout_ = vk::ImageLayout::eUndefined;
	}

	/// Sets the tracked layout to a new value to which the image was transitioned externally.
	/**
	 * The tracked layout is updated immediately.
	 */
	void set_layout_external(vk::ImageLayout layout) {
		layout_ = layout;
	}

	/// Returns the aspect mode of the image.
	image_aspect_mode aspect_mode() const {
		return aspect_mode_;
	}

	/// Returns the dimensionality of the image.
	image_dimension dimension() const {
		return img_dim_;
	}

	/// Returns the vulkan image type of the image.
	vk::ImageType imgage_type() const {
		return img_type_;
	}

	/// Returns true if the image is layered and false if it is not.
	/**
	 * For single cube map image this will return false even though a cube map consists of 6 layers because
	 * there is only a single cube map.
	 */
	bool layered() const {
		return layered_;
	}

	/// Returns the number of layers in the image object.
	uint32_t layers() const {
		return layers_;
	}
};

/// \brief Empty unspecialized definition for image template class. Specializations provide specific
/// constructor signatures and view creation functions.
template <image_dimension img_dim, bool layered, image_aspect_mode img_aspect = image_aspect_mode::color>
class image {
	static_assert(!layered, "Layered images are not yet implemented.");
};

/// Specialization of the image template class for general unlayered images.
template <image_dimension img_dim, image_aspect_mode img_aspect>
class image<img_dim, false, img_aspect> : public base_image {
public:
	/// Defines the type used for the size of the image (unsigned integer, varies in dimensionality).
	using size_type = image_size<img_dim, false>;
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
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

	/// Creates and returns an image view for the image object using the given view parameters.
	image_view<img_dim, false, img_aspect> create_view(uint32_t base_mip_level = 0,
													   uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
													   vk::ComponentMapping component_mapping = {},
													   boost::optional<vk::Format> view_format = {}) {
		return image_view<img_dim, false, img_aspect>(base_image::create_view(
				detail::image_view_type_mapper<img_dim, false, img_aspect>::vk_view_type, 0, 1,
				base_mip_level, mip_levels, component_mapping, view_format));
	}
};
/// Specialization of the image template class for unlayered cube map images.
template <image_aspect_mode img_aspect>
class image<image_dimension::dim_cube, false, img_aspect> : public base_image {
public:
	/// Defines the type used for the size of the image (unsigned integer, varies in dimensionality).
	using size_type = image_size<image_dimension::dim_cube, false>;
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
	image(device& dev, device_memory_manager_interface& mem_mgr,
		  destruction_queue_manager* destruction_manager, vk::Format format, size_type size,
		  uint32_t mip_levels, vk::ImageUsageFlags usage,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  bool preinitialized_layout = false)
			: base_image(image_dimension::dim_cube, false, img_aspect,
						 vk::ImageCreateFlagBits::eCubeCompatible,
						 detail::image_view_type_mapper<image_dimension::dim_cube, false,
														img_aspect>::vk_img_type,
						 dev, mem_mgr, destruction_manager, format, {size.width, size.height, size.depth},
						 size.layers, mip_levels, usage, required_flags, mutable_format, tiling,
						 preinitialized_layout) {}

	/// Creates and returns an image view for the cube map image object using the given view parameters.
	image_view<image_dimension::dim_cube, false, img_aspect>
	create_view(uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
				vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		return image_view<image_dimension::dim_cube, false, img_aspect>(base_image::create_view(
				detail::image_view_type_mapper<image_dimension::dim_cube, false, img_aspect>::vk_view_type, 0,
				1, base_mip_level, mip_levels, component_mapping, view_format));
	}
	/// \brief Creates and returns an image view for a single face in the image object using the given view
	/// parameters.
	image_view<image_dimension::dim_2d, false, img_aspect> create_face_view(
			uint32_t face, uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
			vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		return image_view<image_dimension::dim_2d, false, img_aspect>(base_image::create_view(
				detail::image_view_type_mapper<image_dimension::dim_2d, false, img_aspect>::vk_view_type,
				face, 1, base_mip_level, mip_levels, component_mapping, view_format));
	}
	/// \brief Creates and returns an image view for a all faces in the image object as flat image layers
	/// instead of as a cube map using the given view parameters.
	image_view<image_dimension::dim_2d, true, img_aspect>
	create_faces_view(uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
					  vk::ComponentMapping component_mapping = {},
					  boost::optional<vk::Format> view_format = {}) {
		return image_view<image_dimension::dim_2d, false, img_aspect>(base_image::create_view(
				detail::image_view_type_mapper<image_dimension::dim_2d, false, img_aspect>::vk_view_type, 0,
				6, base_mip_level, mip_levels, component_mapping, view_format));
	}
};

using image_1d = image<image_dimension::dim_1d, false, image_aspect_mode::color>;
using image_2d = image<image_dimension::dim_2d, false, image_aspect_mode::color>;
using image_3d = image<image_dimension::dim_3d, false, image_aspect_mode::color>;
using image_cube = image<image_dimension::dim_cube, false, image_aspect_mode::color>;
using image_1d_layered = image<image_dimension::dim_1d, true, image_aspect_mode::color>;
using image_2d_layered = image<image_dimension::dim_2d, true, image_aspect_mode::color>;
using image_cube_layered = image<image_dimension::dim_cube, true, image_aspect_mode::color>;
using image_2d_ds = image<image_dimension::dim_2d, false, image_aspect_mode::depth_stencil>;

using image_view_1d = image_view<image_dimension::dim_1d, false, image_aspect_mode::color>;
using image_view_2d = image_view<image_dimension::dim_2d, false, image_aspect_mode::color>;
using image_view_3d = image_view<image_dimension::dim_3d, false, image_aspect_mode::color>;
using image_view_cube = image_view<image_dimension::dim_cube, false, image_aspect_mode::color>;
using image_view_1d_layered = image_view<image_dimension::dim_1d, true, image_aspect_mode::color>;
using image_view_2d_layered = image_view<image_dimension::dim_2d, true, image_aspect_mode::color>;
using image_view_cube_layered = image_view<image_dimension::dim_cube, true, image_aspect_mode::color>;
using image_view_2d_ds = image_view<image_dimension::dim_2d, false, image_aspect_mode::depth_stencil>;

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_IMAGE_HPP_ */
