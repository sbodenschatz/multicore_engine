/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image_view.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_IMAGE_VIEW_HPP_
#define MCE_GRAPHICS_IMAGE_VIEW_HPP_

/**
 * \file
 * Defines the image_view class that represents a view of an image.
 */

#include <vulkan/vulkan.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>

namespace mce {
namespace graphics {

/// Defines the options for the dimensionality of an image view.
enum class image_view_dimension { dim_1d, dim_2d, dim_3d, dim_cube };

/// Represents a view of an image object to access the image data.
template <image_view_dimension dimension, bool layered = false>
class image_view {
	vk::UniqueImageView native_view_;
	destruction_queue_manager* destruction_mgr_;
	uint32_t base_mip_level_;
	uint32_t mip_levels_;
	vk::ComponentMapping component_mapping_;
	vk::Format format_;
	uint32_t base_layer_;
	uint32_t layers_;

	image_view(vk::UniqueImageView native_view, destruction_queue_manager* destruction_manager,
			   uint32_t base_mip_level, uint32_t mip_levels, vk::ComponentMapping component_mapping,
			   vk::Format format, uint32_t base_layer = 0, uint32_t layers = 1)
			: native_view_{std::move(native_view)}, destruction_mgr_{destruction_manager},
			  base_mip_level_{base_mip_level}, mip_levels_{mip_levels}, component_mapping_{component_mapping},
			  format_{format}, base_layer_{base_layer}, layers_{layers} {}

public:
	/// Allows move construction.
	image_view(image_view&& other) = default;
	/// Allows move assignment.
	image_view& operator=(image_view&& other) = default;
	/// Destroys the image_view and releases the native resources used by it to the deletion_queue.
	~image_view() {
		if(destruction_mgr_) {
			destruction_mgr_->enqueue(std::move(native_view_));
		}
	}

	/// Returns the starting layer of the view supplied during construction.
	/**
	 * For cube map views a layer is a full cube map consisting of 6 physical layers for the faces.
	 */
	uint32_t base_layer() const {
		return base_layer_;
	}

	/// Returns the starting mip map level given on construction.
	uint32_t base_mip_level() const {
		return base_mip_level_;
	}

	/// Returns the component mapping with which the view was constructed.
	const vk::ComponentMapping& component_mapping() const {
		return component_mapping_;
	}

	/// Returns the data format of the view.
	vk::Format format() const {
		return format_;
	}

	/// Returns the number of layers in the view.
	/**
	 * For cube map views a layer is a full cube map consisting of 6 physical layers for the faces.
	 */
	uint32_t layers() const {
		return layers_;
	}

	/// Returns the number of mip map levels in the view.
	uint32_t mip_levels() const {
		return mip_levels_;
	}

	/// Allows access to the underlying vulkan image view object.
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
