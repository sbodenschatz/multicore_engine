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

} /* namespace graphics */
} /* namespace mce */
