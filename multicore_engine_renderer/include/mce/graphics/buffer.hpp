/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_BUFFER_HPP_
#define MCE_GRAPHICS_BUFFER_HPP_

#include <mce/graphics/device_memory_handle.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class destruction_queue_manager;
class device;

class buffer {
	vk::UniqueBuffer buff_;
	device_memory_handle memory_handle_;
	destruction_queue_manager* destruction_mgr_;
	vk::DeviceSize size_;
	vk::BufferUsageFlags usage_;

public:
	buffer(device& dev, device_memory_manager_interface& mem_mgr,
		   destruction_queue_manager* destruction_manager, vk::DeviceSize size, vk::BufferUsageFlags usage,
		   vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal);
	~buffer();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_BUFFER_HPP_ */
