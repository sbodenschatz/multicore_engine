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
		  vk::ImageUsageFlags usage, vk::ImageLayout layout = vk::ImageLayout::eGeneral,
		  vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
		  bool mutable_format = false, vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		  boost::variant<uint32_t, full_mip_chain> mip_levels = full_mip_chain{})
			: dev_{dev}, format_{format}, size_{size}, usage_{usage}, layout_{layout},
			  mutable_format_{mutable_format}, tiling_{tiling}, mip_levels_{mip_levels} {
		vk::ImageCreateInfo ci(mutable_format ? vk::ImageCreateFlagBits::eMutableFormat
											  : vk::ImageCreateFlags{},
							   vk::ImageType::e2D, format, detail::to_extent_3d(size), 1,
							   static_cast<Image_Type*>(this)->layers(), vk::SampleCountFlagBits::e1, tiling,
							   usage, vk::SharingMode::eExclusive);
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
#ifdef DOXYGEN
public:
#else
protected:
#endif
	typedef image<Image_Type, Size_Type> base_t;
	using base_t::image;

public:
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

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_IMAGE_HPP_ */
