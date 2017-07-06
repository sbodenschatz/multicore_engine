/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_OLD_HPP_
#define MCE_GRAPHICS_IMAGE_OLD_HPP_

/**
 * \file
 * Defines the classes that manage images and their associated storage.
 */

#ifdef DOXYGEN
/// Workaround for Doxygen not correctly handling using declarations.
#define DOXYGEN_ONLY_PUBLIC(REAL) public
#else
#define DOXYGEN_ONLY_PUBLIC(REAL) REAL
#endif

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <mce/graphics/image_view_old.hpp>
#include <mce/util/math_tools.hpp>
#include <type_traits>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class image_1d;
class image_2d;
class image_3d;
class image_cube;
class image_1d_layered;
class image_2d_layered;
class image_cube_layered;

template <image_view_dimension dimension, bool layered>
class image_view;

namespace detail {

template <int component_count>
struct extent_converter {};

template <>
struct extent_converter<1> {
	template <typename T>
	static vk::Extent3D convert_to_extent_3d(const T& v) {
		return {v, 1, 1};
	}
};

template <>
struct extent_converter<2> {
	template <typename T>
	static vk::Extent3D convert_to_extent_3d(const T& v) {
		return {v.x, v.y, 1};
	}
};
template <>
struct extent_converter<3> {
	template <typename T>
	static vk::Extent3D convert_to_extent_3d(const T& v) {
		return {v.x, v.y, v.z};
	}
};

template <typename T, glm::precision p, template <typename, glm::precision> class Vector_Type>
vk::Extent3D to_extent_3d(const Vector_Type<T, p>& v) {
	return extent_converter<util::vector_size<Vector_Type<T, p>>::value>::convert_to_extent_3d(v);
}
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
vk::Extent3D to_extent_3d(T v) {
	return extent_converter<1>::convert_to_extent_3d(v);
}

template <typename Img>
struct type_mapper {};

template <>
struct type_mapper<image_1d> {
	using flat_view = image_view<image_view_dimension::dim_1d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e1D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::e1D;
	static constexpr uint32_t cube_layer_factor = 1;
	static vk::ImageCreateFlags base_flags() {
		return {};
	}
};

template <>
struct type_mapper<image_2d> {
	using flat_view = image_view<image_view_dimension::dim_2d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::e2D;
	static constexpr uint32_t cube_layer_factor = 1;
	static vk::ImageCreateFlags base_flags() {
		return {};
	}
};

template <>
struct type_mapper<image_3d> {
	using flat_view = image_view<image_view_dimension::dim_3d>;
	using layered_side_view = image_view<image_view_dimension::dim_2d, true>;
	static constexpr vk::ImageType img_type = vk::ImageType::e3D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::e3D;
	static constexpr vk::ImageViewType layered_view_type = vk::ImageViewType::e2DArray;
	static constexpr uint32_t cube_layer_factor = 1;
	static vk::ImageCreateFlags base_flags() {
		return vk::ImageCreateFlagBits::e2DArrayCompatibleKHR;
	}
};

template <>
struct type_mapper<image_cube> {
	using flat_view = image_view<image_view_dimension::dim_cube>;
	using layered_side_view = image_view<image_view_dimension::dim_2d, true>;
	using side_view = image_view<image_view_dimension::dim_2d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::eCube;
	static constexpr vk::ImageViewType layered_side_view_type = vk::ImageViewType::e2DArray;
	static constexpr vk::ImageViewType side_view_type = vk::ImageViewType::e2D;
	static constexpr uint32_t cube_layer_factor = 6;
	static vk::ImageCreateFlags base_flags() {
		return vk::ImageCreateFlagBits::eCubeCompatible;
	}
};

template <>
struct type_mapper<image_1d_layered> {
	using layered_view = image_view<image_view_dimension::dim_1d, true>;
	using flat_view = image_view<image_view_dimension::dim_1d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e1D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::e1D;
	static constexpr vk::ImageViewType layered_view_type = vk::ImageViewType::e1DArray;
	static constexpr uint32_t cube_layer_factor = 1;
	static vk::ImageCreateFlags base_flags() {
		return {};
	}
};

template <>
struct type_mapper<image_2d_layered> {
	using layered_view = image_view<image_view_dimension::dim_2d, true>;
	using flat_view = image_view<image_view_dimension::dim_2d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::e2D;
	static constexpr vk::ImageViewType layered_view_type = vk::ImageViewType::e2DArray;
	static constexpr uint32_t cube_layer_factor = 1;
	static vk::ImageCreateFlags base_flags() {
		return {};
	}
};

template <>
struct type_mapper<image_cube_layered> {
	using flat_view = image_view<image_view_dimension::dim_cube>;
	using layered_view = image_view<image_view_dimension::dim_cube, true>;
	using layered_side_view = image_view<image_view_dimension::dim_2d, true>;
	using side_view = image_view<image_view_dimension::dim_2d>;
	static constexpr vk::ImageType img_type = vk::ImageType::e2D;
	static constexpr vk::ImageViewType flat_view_type = vk::ImageViewType::eCube;
	static constexpr vk::ImageViewType layered_view_type = vk::ImageViewType::eCubeArray;
	static constexpr vk::ImageViewType layered_side_view_type = vk::ImageViewType::e2DArray;
	static constexpr vk::ImageViewType side_view_type = vk::ImageViewType::e2D;
	static constexpr uint32_t cube_layer_factor = 6;
	static vk::ImageCreateFlags base_flags() {
		return vk::ImageCreateFlagBits::eCubeCompatible;
	}
};

} // namespace detail

template <typename Image_Type, typename Size_Type>
class single_image;

/// Defines the aspect modes for an, specifying which aspect(s) it consists of.
enum class image_aspect_mode { color, depth, stencil, depth_stencil };

/// Implementation base for all image classes defining the common functionality for images.
/**
 * \warning Not to be used polymorphically.
 */
template <typename Image_Type, typename Size_Type>
class image {
	device& dev_;
	destruction_queue_manager* destruction_mgr_;
	vk::Format format_;
	Size_Type size_;
	vk::ImageUsageFlags usage_;
	vk::ImageLayout layout_;
	bool mutable_format_;
	vk::ImageTiling tiling_;
	uint32_t mip_levels_;
	device_memory_handle mem_handle_;
	vk::UniqueImage img_;
	image_aspect_mode aspect_mode_;

public:
	/// Defines the type used for the size of the image (unsigned integer, varies in dimensionality).
	using size_type = Size_Type;

	/// Tag type to signal to the constructor, that the image should use the full chain of mip map levels.
	struct full_mip_chain {};

#ifdef DOXYGEN
public:
#else
protected:
#endif
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
	image(device& dev, device_memory_manager_interface& mem_mgr,
		  destruction_queue_manager* destruction_manager, vk::Format format, size_type size,
		  vk::ImageUsageFlags usage, uint32_t layers,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  bool preinitialized_layout = false,
		  boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{},
		  image_aspect_mode aspect_mode = image_aspect_mode::color)
			: dev_{dev}, destruction_mgr_{destruction_manager}, format_{format}, size_{size}, usage_{usage},
			  layout_{preinitialized_layout ? vk::ImageLayout::ePreinitialized : vk::ImageLayout::eUndefined},
			  mutable_format_{mutable_format}, tiling_{tiling}, mip_levels_{1}, aspect_mode_{aspect_mode} {
		vk::ImageCreateInfo ci(
				(mutable_format ? vk::ImageCreateFlagBits::eMutableFormat : vk::ImageCreateFlags{}) |
						detail::type_mapper<Image_Type>::base_flags(),
				detail::type_mapper<Image_Type>::img_type, format, detail::to_extent_3d(size), 1,
				layers * detail::type_mapper<Image_Type>::cube_layer_factor, vk::SampleCountFlagBits::e1,
				tiling, usage, vk::SharingMode::eExclusive, 0, nullptr, layout_);
		struct mip_visitor : boost::static_visitor<> {
			uint32_t& mip_levels;
			typename Image_Type::size_type size_;
			mip_visitor(uint32_t& mip_levels, typename Image_Type::size_type size)
					: mip_levels{mip_levels}, size_{size} {}
			void operator()(uint32_t mips) const {
				mip_levels = mips;
			}
			void operator()(full_mip_chain) const {
				mip_levels = uint32_t(1 + floor(log2(util::component_max(size_))));
			}
		};
		mip_visitor mv{mip_levels_, size};
		mip_levels.apply_visitor(mv);
		img_ = dev.native_device().createImageUnique(ci);
		mem_handle_ = make_device_memory_handle(
				mem_mgr,
				mem_mgr.allocate(dev.native_device().getImageMemoryRequirements(*img_), required_flags));
		dev.native_device().bindImageMemory(*img_, mem_handle_.memory(), mem_handle_.offset());
	}

public:
	/// Destroys the image object and releases it associated resources to the deletion queue.
	~image() {
		if(destruction_mgr_) {
			destruction_mgr_->enqueue(std::move(img_));
			destruction_mgr_->enqueue(std::move(mem_handle_));
		}
	}

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
	Size_Type size() const {
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

	/// Returns the aspect flags for the aspect mode of the image.
	/**
	 * For color, depth, and stencil only the respective flag is set.
	 * For depth_stencil both the depth and stencil flag are set.
	 * The returned aspect flags set is the usual value used for views into images of this aspect mode.
	 */
	vk::ImageAspectFlags default_aspect_flags() const {
		if(aspect_mode_ == image_aspect_mode::color) {
			return vk::ImageAspectFlagBits::eColor;
		} else if(aspect_mode_ == image_aspect_mode::depth) {
			return vk::ImageAspectFlagBits::eDepth;
		} else if(aspect_mode_ == image_aspect_mode::stencil) {
			return vk::ImageAspectFlagBits::eStencil;
		} else if(aspect_mode_ == image_aspect_mode::depth_stencil) {
			return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		} else {
			throw std::logic_error("Invalid aspect_mode.");
		}
	}

	/// Returns the parent device.
	const device& dev() const {
		return dev_;
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
				*img_, vk::ImageSubresourceRange(default_aspect_flags(), 0, mip_levels_, 0,
												 static_cast<Image_Type*>(this)->layers() *
														 detail::type_mapper<Image_Type>::cube_layer_factor));
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

	/// Provides access to the associated destruction queue manager for derived classes.
	destruction_queue_manager* destruction_mgr() const {
		return destruction_mgr_;
	}
};

/// Implementation base class for non-array images.
/**
 * \warning Not to be used polymorphically.
 */
template <typename Image_Type, typename Size_Type>
class single_image : DOXYGEN_ONLY_PUBLIC(protected) image<Image_Type, Size_Type> {
public:
	/// Reflects the type of the base class.
	typedef image<Image_Type, Size_Type> base_t;
	friend class image_1d;
	friend class image_2d;
	friend class image_3d;

	/// Returns the number of layers in the image (always 1 for single_image).
	/**
	 * A cube map also counts as one layer here even though it consists of multiple physical layers.
	 */
	uint32_t layers() const {
		return 1;
	}

	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;

protected:
	using base_t::default_aspect_flags;
	using base_t::dev;
	using base_t::destruction_mgr;

public:
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
	single_image(device& dev, device_memory_manager_interface& mem_mgr,
				 destruction_queue_manager* destruction_manager, vk::Format format,
				 typename base_t::size_type size, vk::ImageUsageFlags usage,
				 vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
				 bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
				 bool preinitialized_layout = false,
				 boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{},
				 image_aspect_mode aspect_mode = image_aspect_mode::color)
			: base_t(dev, mem_mgr, destruction_manager, format, size, usage, 1, required_flags,
					 mutable_format, tiling, preinitialized_layout, mip_levels, aspect_mode) {}

	/// Creates and returns an image view for the image object using the given view parameters.
	typename detail::type_mapper<Image_Type>::flat_view
	create_view(uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
				vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(), detail::type_mapper<Image_Type>::flat_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels, 0,
									detail::type_mapper<Image_Type>::cube_layer_factor});

		return typename detail::type_mapper<Image_Type>::flat_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format);
	}
};

/// Implementation base class for array images.
/**
 * \warning Not to be used polymorphically.
 */
template <typename Image_Type, typename Size_Type>
class layered_image : DOXYGEN_ONLY_PUBLIC(protected) image<Image_Type, Size_Type> {
	uint32_t layers_;

public:
	/// Reflects the type of the base class.
	typedef image<Image_Type, Size_Type> base_t;
	friend class image_1d;
	friend class image_2d;

	/// Returns the number of array image layers in the image.
	/**
	 * A cube map also counts as one layer here even though it consists of multiple physical layers.
	 */
	uint32_t layers() const {
		return layers_;
	}

	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;

protected:
	using base_t::default_aspect_flags;
	using base_t::dev;
	using base_t::destruction_mgr;

public:
	/// Constructs an image using the given parameters and associates it with memory from the given manager.
	layered_image(device& dev, device_memory_manager_interface& mem_mgr,
				  destruction_queue_manager* destruction_manager, vk::Format format,
				  typename base_t::size_type size, vk::ImageUsageFlags usage, uint32_t layers,
				  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
				  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
				  bool preinitialized_layout = false,
				  boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{},
				  image_aspect_mode aspect_mode = image_aspect_mode::color)
			: base_t(dev, mem_mgr, destruction_manager, format, size, usage, layers, required_flags,
					 mutable_format, tiling, preinitialized_layout, mip_levels, aspect_mode),
			  layers_{layers} {}

	/// Creates and returns a layered image view for the image object using the given view parameters.
	typename detail::type_mapper<Image_Type>::layered_view
	create_view(uint32_t base_layer = 0, uint32_t layers = VK_REMAINING_ARRAY_LAYERS,
				uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
				vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(), detail::type_mapper<Image_Type>::flat_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels,
									base_layer * detail::type_mapper<Image_Type>::cube_layer_factor,
									VK_REMAINING_ARRAY_LAYERS});
		if(layers != VK_REMAINING_ARRAY_LAYERS) {
			ci.subresourceRange.setLayerCount(layers * detail::type_mapper<Image_Type>::cube_layer_factor);
		}

		return typename detail::type_mapper<Image_Type>::layered_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, base_layer, layers);
	}
	/// \brief Creates and returns an image view of a single layer of the image object using the given view
	/// parameters.
	typename detail::type_mapper<Image_Type>::flat_view create_single_layer_view(
			uint32_t layer, uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
			vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(), detail::type_mapper<Image_Type>::flat_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels,
									layer * detail::type_mapper<Image_Type>::cube_layer_factor,
									detail::type_mapper<Image_Type>::cube_layer_factor});

		return typename detail::type_mapper<Image_Type>::flat_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, layer);
	}
};

/// Represents a 1-dimensional non-array image object with the associated resources.
class image_1d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_1d, uint32_t> {
	typedef single_image<image_1d, uint32_t> base_t;
	friend class single_image<image_1d, uint32_t>;

public:
	using base_t::single_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;
};

/// Represents a 2-dimensional non-array image object with the associated resources.
class image_2d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_2d, glm::uvec2> {
	typedef single_image<image_2d, glm::uvec2> base_t;
	friend class single_image<image_2d, glm::uvec2>;

public:
	using base_t::single_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;
};

/// Represents a 3-dimensional non-array image object with the associated resources.
class image_3d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_3d, glm::uvec3> {
	typedef single_image<image_3d, glm::uvec3> base_t;
	friend class single_image<image_3d, glm::uvec3>;

public:
	using base_t::single_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;
};

/// Represents a non-array cube map image object with the associated resources.
class image_cube : DOXYGEN_ONLY_PUBLIC(private) single_image<image_cube, uint32_t> {
	typedef single_image<image_cube, uint32_t> base_t;
	friend class single_image<image_cube, uint32_t>;
	using Image_Type = image_cube;

public:
	using base_t::single_image;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;

	/// \brief Creates and returns an image view of a single side of the cube map image object using the given
	/// view parameters.
	typename detail::type_mapper<Image_Type>::side_view create_single_side_view(
			uint32_t layer, uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
			vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(), detail::type_mapper<Image_Type>::side_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels, layer, 1});

		return typename detail::type_mapper<Image_Type>::side_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, layer);
	}
	/// \brief Creates and returns an image view of sides of the cube map image object using the given
	/// view parameters.
	/**
	 * Each side forms a separate layer.
	 */
	typename detail::type_mapper<Image_Type>::layered_side_view
	create_layered_side_view(uint32_t base_layer = 0, uint32_t layers = VK_REMAINING_ARRAY_LAYERS,
							 uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
							 vk::ComponentMapping component_mapping = {},
							 boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(),
								   detail::type_mapper<Image_Type>::layered_side_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels, base_layer, layers});

		return typename detail::type_mapper<Image_Type>::layered_side_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, base_layer, layers);
	}
};

/// Represents a 1-dimensional array image object with the associated resources.
class image_1d_layered : DOXYGEN_ONLY_PUBLIC(private) layered_image<image_1d_layered, uint32_t> {
	typedef layered_image<image_1d_layered, uint32_t> base_t;
	friend class layered_image<image_1d_layered, uint32_t>;

public:
	using base_t::layered_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using base_t::create_single_layer_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;
};

/// Represents a 1-dimensional array image object with the associated resources.
class image_2d_layered : DOXYGEN_ONLY_PUBLIC(private) layered_image<image_2d_layered, glm::uvec2> {
	typedef layered_image<image_2d_layered, glm::uvec2> base_t;
	friend class layered_image<image_2d_layered, glm::uvec2>;

public:
	using base_t::layered_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using base_t::create_single_layer_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;
};

/// Represents an array cube map image object with the associated resources.
class image_cube_layered : DOXYGEN_ONLY_PUBLIC(private) layered_image<image_cube_layered, uint32_t> {
	typedef layered_image<image_cube_layered, uint32_t> base_t;
	friend class layered_image<image_cube_layered, uint32_t>;
	using Image_Type = image_cube;

public:
	using base_t::layered_image;
	using base_t::layers;
	using base_t::format;
	using base_t::mip_levels;
	using base_t::mutable_format;
	using base_t::native_image;
	using base_t::size;
	using base_t::tiling;
	using base_t::tracked_layout;
	using base_t::usage;
	using base_t::create_view;
	using base_t::create_single_layer_view;
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
	using base_t::generate_transition;

	/// \brief Creates and returns an image view of a single side of the cube map image object using the given
	/// view parameters.
	typename detail::type_mapper<Image_Type>::side_view create_single_side_view(
			uint32_t layer, uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
			vk::ComponentMapping component_mapping = {}, boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(), detail::type_mapper<Image_Type>::side_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels, layer, 1});

		return typename detail::type_mapper<Image_Type>::side_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, layer);
	}
	/// \brief Creates and returns an image view of sides of the cube map image object using the given
	/// view parameters.
	/**
	 * Each side forms a separate layer.
	 */
	typename detail::type_mapper<Image_Type>::layered_side_view
	create_layered_side_view(uint32_t base_layer = 0, uint32_t layers = VK_REMAINING_ARRAY_LAYERS,
							 uint32_t base_mip_level = 0, uint32_t mip_levels = VK_REMAINING_MIP_LEVELS,
							 vk::ComponentMapping component_mapping = {},
							 boost::optional<vk::Format> view_format = {}) {
		vk::ImageViewCreateInfo ci({}, native_image(),
								   detail::type_mapper<Image_Type>::layered_side_view_type,
								   view_format.value_or(format()), component_mapping,
								   {default_aspect_flags(), base_mip_level, mip_levels, base_layer, layers});

		return typename detail::type_mapper<Image_Type>::layered_side_view(
				dev().native_device().createImageViewUnique(ci), destruction_mgr(), base_mip_level,
				mip_levels, component_mapping, ci.format, base_layer, layers);
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_IMAGE_OLD_HPP_ */
