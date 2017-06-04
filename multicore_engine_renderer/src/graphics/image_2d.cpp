/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/image.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/image_2d.hpp>

namespace mce {
namespace graphics {

image::image(device& dev, device_memory_manager& mem_mgr, vk::Format format, glm::uvec2 size,
			 vk::ImageUsageFlags usage, vk::ImageLayout layout, vk::MemoryPropertyFlags required_flags,
			 bool mutable_format, vk::ImageTiling tiling, uint32_t mip_levels, uint32_t layers)
		: format_{format}, size_{size}, usage_{usage}, layout_{layout}, mutable_format_{mutable_format},
		  tiling_{tiling}, mip_levels_{mip_levels}, layers_{layers} {
	vk::ImageCreateInfo ci(mutable_format ? vk::ImageCreateFlagBits::eMutableFormat : vk::ImageCreateFlags{},
						   vk::ImageType::e2D, format, {size.x, size.y, 1}, mip_levels, layers,
						   vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive);
	ci.setInitialLayout(layout);
	img_ = dev.native_device().createImageUnique(ci);
	mem_handle_ = make_device_memory_handle(
			mem_mgr, mem_mgr.allocate(dev.native_device().getImageMemoryRequirements(*img_), required_flags));
	dev.native_device().bindImageMemory(*img_, mem_handle_.memory(), mem_handle_.offset());
}

} /* namespace graphics */
} /* namespace mce */
