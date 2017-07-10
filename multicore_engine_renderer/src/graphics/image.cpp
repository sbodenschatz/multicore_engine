/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/image.hpp>

namespace mce {
namespace graphics {

base_image::~base_image() noexcept {}
base_image::base_image(image_dimension img_dim, bool layered, image_aspect_mode aspect_mode,
					   vk::ImageCreateFlags base_create_flags, vk::ImageType img_type, device& dev,
					   device_memory_manager_interface& mem_mgr,
					   destruction_queue_manager* destruction_manager, vk::Format format, vk::Extent3D size,
					   uint32_t layers, uint32_t mip_levels, vk::ImageUsageFlags usage,
					   vk::MemoryPropertyFlags required_flags, bool mutable_format, vk::ImageTiling tiling,
					   bool preinitialized_layout)
		: img_dim_{img_dim}, layered_{layered}, aspect_mode_{aspect_mode}, img_type_{img_type}, dev_{&dev},
		  format_{format}, size_{size}, layers_{layers}, mip_levels_{mip_levels}, usage_{usage},
		  required_flags_{required_flags}, mutable_format_{mutable_format}, tiling_{tiling},
		  layout_{preinitialized_layout ? vk::ImageLayout::ePreinitialized : vk::ImageLayout::eUndefined} {
	vk::ImageCreateInfo ci(
			(mutable_format ? vk::ImageCreateFlagBits::eMutableFormat : vk::ImageCreateFlags{}) |
					base_create_flags,
			img_type, format, size, mip_levels, layers, vk::SampleCountFlagBits::e1, tiling, usage,
			vk::SharingMode::eExclusive, 0, nullptr, layout_);
	img_ = decltype(img_)(dev.native_device().createImageUnique(ci), destruction_manager);
	mem_handle_ = decltype(mem_handle_)(
			make_device_memory_handle(
					mem_mgr,
					mem_mgr.allocate(dev.native_device().getImageMemoryRequirements(*img_), required_flags)),
			destruction_manager);
	dev.native_device().bindImageMemory(*img_, mem_handle_->memory(), mem_handle_->offset());
}

vk::ImageAspectFlags base_image::default_aspect_flags() const {
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

any_image_view base_image::create_view(vk::ImageViewType view_type, uint32_t base_layer, uint32_t view_layers,
									   uint32_t base_mip_level, uint32_t mip_levels,
									   vk::ComponentMapping component_mapping,
									   boost::optional<vk::Format> view_format) {
	vk::ImageViewCreateInfo ci({}, *img_, view_type, view_format.value_or(format_), component_mapping,
							   {default_aspect_flags(), base_mip_level, mip_levels, base_layer, view_layers});

	return any_image_view(queued_handle<vk::UniqueImageView>(dev_->native_device().createImageViewUnique(ci),
															 img_.destruction_manager()),
						  base_mip_level, mip_levels, component_mapping, ci.format, base_layer,
						  (view_layers == VK_REMAINING_ARRAY_LAYERS) ? layers_ : view_layers);
}

base_image_view::~base_image_view() noexcept {}
base_image_view::base_image_view(queued_handle<vk::UniqueImageView> view, uint32_t base_mip_level,
								 uint32_t mip_levels, vk::ComponentMapping component_mapping,
								 vk::Format format, uint32_t base_layer, uint32_t layers)
		: view_{std::move(view)}, base_mip_level_{base_mip_level}, mip_levels_{mip_levels},
		  component_mapping_{component_mapping}, format_{format}, base_layer_{base_layer}, layers_{layers} {}
any_image_view::any_image_view(queued_handle<vk::UniqueImageView> view, uint32_t base_mip_level,
							   uint32_t mip_levels, vk::ComponentMapping component_mapping, vk::Format format,
							   uint32_t base_layer, uint32_t layers)
		: base_image_view(std::move(view), base_mip_level, mip_levels, component_mapping, format, base_layer,
						  layers) {}

} /* namespace graphics */
} /* namespace mce */
