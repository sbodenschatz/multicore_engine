/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/buffer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_BUFFER_HPP_
#define MCE_GRAPHICS_BUFFER_HPP_

/**
 * \file
 * Provides the definition of the wrapper class for buffer objects.
 */

#include <mce/graphics/device_memory_handle.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class destruction_queue_manager;
class device;

/// Implements buffer objects, with which arbitrary data can be kept in device memory for different purposes.
class buffer {
	vk::UniqueBuffer buff_;
	device_memory_handle memory_handle_;
	destruction_queue_manager* destruction_mgr_;
	vk::DeviceSize size_;
	vk::BufferUsageFlags usage_;

public:
	/// Constructs a buffer with the given parameters.
	buffer(device& dev, device_memory_manager_interface& mem_mgr,
		   destruction_queue_manager* destruction_manager, vk::DeviceSize size, vk::BufferUsageFlags usage,
		   vk::MemoryPropertyFlags required_flags = vk::MemoryPropertyFlagBits::eDeviceLocal);
	/// Destroys the buffer wrapper object and releases the used resources to the destruction queue.
	~buffer();

	// TODO: Implement
	buffer(buffer&&);
	buffer& operator=(buffer&&);

	/// Allows access to the underlying native buffer object handle.
	vk::Buffer native_buffer() {
		return buff_.get();
	}

	/// Allows access to the underlying native buffer object handle.
	const vk::Buffer& native_buffer() const {
		return *buff_;
	}

	/// Returns the size of the buffer object.
	vk::DeviceSize size() const {
		return size_;
	}

	/// Returns the allowed / requested usage modes for the buffer object.
	vk::BufferUsageFlags usage() const {
		return usage_;
	}

	/// Allows host access to the buffer content if it is host-visible.
	const void* mapped_pointer() const {
		return memory_handle_.mapped_pointer();
	}
	/// Allows host access to the buffer content if it is host-visible.
	void* mapped_pointer() {
		return memory_handle_.mapped_pointer();
	}
	/// Flushes the non-coherent buffer content from the host.
	void flush_mapped(vk::Device& dev, vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE);
	/// Invalidates non-coherent buffer content on the host.
	void invalidate_mapped(vk::Device& dev, vk::DeviceSize offset = 0, vk::DeviceSize size = VK_WHOLE_SIZE);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_BUFFER_HPP_ */
