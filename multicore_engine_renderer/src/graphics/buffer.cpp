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
		: size_{size}, usage_{usage} {
	vk::BufferCreateInfo ci({}, size, usage, vk::SharingMode::eExclusive);
	buff_ = decltype(buff_)(dev.native_device().createBufferUnique(ci), destruction_manager);
	memory_handle_ = decltype(memory_handle_)(
			make_device_memory_handle(
					mem_mgr,
					mem_mgr.allocate(dev.native_device().getBufferMemoryRequirements(*buff_),
									 required_flags)),
			destruction_manager);
}

buffer::~buffer() {}

buffer::buffer(buffer&& other)
		: buff_{std::move(other.buff_)}, memory_handle_{std::move(other.memory_handle_)},
		  destruction_mgr_{std::move(other.destruction_mgr_)}, size_{other.size_}, usage_{other.usage_} {
	other.destruction_mgr_ = nullptr;
	other.size_ = 0;
	other.usage_ = {};
}
buffer& buffer::operator=(buffer&& other) {
	if(destruction_mgr_) {
		destruction_mgr_->enqueue(std::move(buff_));
		destruction_mgr_->enqueue(std::move(memory_handle_));
	}
	buff_ = std::move(other.buff_);
	memory_handle_ = std::move(other.memory_handle_);
	destruction_mgr_ = std::move(other.destruction_mgr_);
	size_ = other.size_;
	usage_ = other.usage_;
	other.destruction_mgr_ = nullptr;
	other.size_ = 0;
	other.usage_ = {};
	return *this;
}

void buffer::flush_mapped(vk::Device& dev, vk::DeviceSize offset, vk::DeviceSize size) {
	memory_handle_->flush_mapped(dev, offset, size);
}
void buffer::invalidate_mapped(vk::Device& dev, vk::DeviceSize offset, vk::DeviceSize size) {
	memory_handle_->invalidate_mapped(dev, offset, size);
}

} /* namespace graphics */
} /* namespace mce */
