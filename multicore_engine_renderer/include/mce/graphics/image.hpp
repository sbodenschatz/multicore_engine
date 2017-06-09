/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_HPP_
#define MCE_GRAPHICS_IMAGE_HPP_

#ifdef DOXYGEN
#define DOXYGEN_ONLY_PUBLIC(REAL) public
#else
#define DOXYGEN_ONLY_PUBLIC(REAL) REAL
#endif

#include <boost/variant.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/device_memory_handle.hpp>
#include <mce/util/math_tools.hpp>
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

class tag_1d {};
class tag_2d {};
class tag_3d {};
class tag_cube {};

template <typename Tag, bool layered = false>
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

template <typename T, typename P, P p, template <typename, P> class Vector_Type>
vk::Extent3D to_extent_3d(const Vector_Type<T, p>& v) {
	return extent_converter<util::vector_size(v)>::convert_to_extent_3d();
}

template <typename Img>
struct view_types {};

template <>
struct view_types<image_1d> {
	using flat_view = image_view<tag_1d>;
};

template <>
struct view_types<image_2d> {
	using flat_view = image_view<tag_2d>;
};
template <>
struct view_types<image_3d> {
	using flat_view = image_view<tag_3d>;
};
template <>
struct view_types<image_cube> {
	using cube_view = image_view<tag_cube>;
	using layered_view = image_view<tag_2d, true>;
	using side_view = image_view<tag_2d>;
};

template <>
struct view_types<image_2d_layered> {
	using layered_view = image_view<tag_2d, true>;
	using flat_view = image_view<tag_2d>;
};

template <>
struct view_types<image_1d_layered> {
	using layered_view = image_view<tag_1d, true>;
	using flat_view = image_view<tag_1d>;
};

} // namespace detail

template <typename Image_Type, typename Size_Type>
class single_image;

template <typename Image_Type, typename Size_Type>
class image {
	device& dev_;
	vk::Format format_;
	Size_Type size_;
	vk::ImageUsageFlags usage_;
	vk::ImageLayout layout_;
	bool mutable_format_;
	vk::ImageTiling tiling_;
	uint32_t mip_levels_;
	device_memory_handle mem_handle_;
	vk::UniqueImage img_;

public:
	using size_type = Size_Type;

	struct full_mip_chain {};

#ifdef DOXYGEN
public:
#else
protected:
#endif
	image(device& dev, device_memory_manager_interface& mem_mgr, vk::Format format, size_type size,
		  vk::ImageUsageFlags usage, uint32_t layers, vk::ImageLayout layout = vk::ImageLayout::eGeneral,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{})
			: dev_{dev}, format_{format}, size_{size}, usage_{usage}, layout_{layout},
			  mutable_format_{mutable_format}, tiling_{tiling}, mip_levels_{mip_levels} {
		vk::ImageCreateInfo ci(mutable_format ? vk::ImageCreateFlagBits::eMutableFormat
											  : vk::ImageCreateFlags{},
							   vk::ImageType::e2D, format, detail::to_extent_3d(size), 1, layers,
							   vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive);
		struct mip_visitor : boost::static_visitor<> {
			uint32_t& mip_levels;
			typename Image_Type::size_type size_;
			void operator()(uint32_t mips) const {
				mip_levels = mips;
			}
			void operator()(full_mip_chain) const {
				mip_levels = 1 + floor(log2(util::component_max(size_)));
			}
		};
		mip_visitor mv{mip_levels_, size};
		mip_levels.apply_visitor(mv);
		ci.setInitialLayout(layout);
		img_ = dev.native_device().createImageUnique(ci);
		mem_handle_ = make_device_memory_handle(
				mem_mgr,
				mem_mgr.allocate(dev.native_device().getImageMemoryRequirements(*img_), required_flags));
		dev.native_device().bindImageMemory(*img_, mem_handle_.memory(), mem_handle_.offset());
	}

public:
	~image() {
		// TODO: Insert resources into deletion manager.
	}

	friend class single_image<Image_Type, Size_Type>;

	vk::Format format() const {
		return format_;
	}

	const vk::Image& native_image() const {
		return *img_;
	}

	vk::ImageLayout tracked_layout() const {
		return layout_;
	}

	uint32_t mip_levels() const {
		return mip_levels_;
	}

	bool mutable_format() const {
		return mutable_format_;
	}

	Size_Type size() const {
		return size_;
	}

	vk::ImageTiling tiling() const {
		return tiling_;
	}

	vk::ImageUsageFlags usage() const {
		return usage_;
	}
};

template <typename Image_Type, typename Size_Type>
class single_image : DOXYGEN_ONLY_PUBLIC(protected) image<Image_Type, Size_Type> {
public:
	typedef image<Image_Type, Size_Type> base_t;
	friend class image_1d;
	friend class image_2d;
	friend class image_3d;

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

#ifdef DOXYGEN
public:
#else
protected:
#endif
	single_image(device& dev, device_memory_manager_interface& mem_mgr, vk::Format format,
				 typename base_t::size_type size, vk::ImageUsageFlags usage,
				 vk::ImageLayout layout = vk::ImageLayout::eGeneral,
				 vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
				 bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
				 boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{})
			: base_t(dev, mem_mgr, format, size, usage, static_cast<Image_Type*>(this)->layers(), layout,
					 required_flags, mutable_format, tiling, mip_levels) {}
};

template <typename Image_Type, typename Size_Type>
class layered_image : DOXYGEN_ONLY_PUBLIC(protected) image<Image_Type, Size_Type> {
	uint32_t layers_;

public:
	typedef image<Image_Type, Size_Type> base_t;
	friend class image_1d;
	friend class image_2d;

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

#ifdef DOXYGEN
public:
#else
protected:
#endif
	layered_image(device& dev, device_memory_manager_interface& mem_mgr, vk::Format format,
				  typename base_t::size_type size, vk::ImageUsageFlags usage, uint32_t layers,
				  vk::ImageLayout layout = vk::ImageLayout::eGeneral,
				  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
				  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
				  boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{})
			: base_t(dev, mem_mgr, format, size, usage, layers, layout, required_flags, mutable_format,
					 tiling, mip_levels),
			  layers_{layers} {}
};
class image_1d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_1d, uint32_t> {
	typedef single_image<image_1d, uint32_t> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
};

class image_2d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_2d, glm::uvec2> {
	typedef single_image<image_2d, glm::uvec2> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
};

class image_3d : DOXYGEN_ONLY_PUBLIC(private) single_image<image_3d, glm::uvec3> {
	typedef single_image<image_3d, glm::uvec3> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
};

class image_cube : DOXYGEN_ONLY_PUBLIC(private) single_image<image_cube, uint32_t> {
	typedef single_image<image_cube, uint32_t> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;

	uint32_t layers() const {
		return 6;
	}
};

class image_1d_layered : DOXYGEN_ONLY_PUBLIC(private) single_image<image_1d_layered, uint32_t> {
	typedef single_image<image_1d_layered, uint32_t> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
};

class image_2d_layered : DOXYGEN_ONLY_PUBLIC(private) single_image<image_2d_layered, glm::uvec2> {
	typedef single_image<image_2d_layered, glm::uvec2> base_t;

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
	using typename base_t::size_type;
	using typename base_t::full_mip_chain;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_IMAGE_HPP_ */
