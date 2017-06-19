/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/buffer.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/buffer.hpp>
#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

buffer::buffer(device& dev, device_memory_manager_interface& mem_mgr,
			   destruction_queue_manager* destruction_manager, vk::DeviceSize size,
			   vk::BufferUsageFlags usage, vk::MemoryPropertyFlags required_flags)
		: destruction_mgr_{destruction_manager}, size_{size}, usage_{usage} {
	vk::BufferCreateInfo ci({}, size, usage, vk::SharingMode::eExclusive);
	buff_ = dev.native_device().createBufferUnique(ci);
	memory_handle_ = make_device_memory_handle(
			mem_mgr,
			mem_mgr.allocate(dev.native_device().getBufferMemoryRequirements(*buff_), required_flags));
}

buffer::~buffer() {
	if(destruction_mgr_) {
		destruction_mgr_->enqueue(std::move(buff_));
		destruction_mgr_->enqueue(std::move(memory_handle_));
	}
}

} /* namespace graphics */
} /* namespace mce */
