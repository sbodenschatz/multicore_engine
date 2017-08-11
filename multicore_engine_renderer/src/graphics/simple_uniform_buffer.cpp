/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/simple_uniform_buffer.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/simple_uniform_buffer.hpp>

namespace mce {
namespace graphics {

simple_uniform_buffer::simple_uniform_buffer(device& dev, device_memory_manager_interface& mem_mgr,
											 destruction_queue_manager* destruction_manager,
											 vk::DeviceSize size)
		: data_buffer_(dev, mem_mgr, destruction_manager, size, vk::BufferUsageFlagBits::eUniformBuffer,
					   vk::MemoryPropertyFlagBits::eHostVisible),
		  current_offset_{0} {
	// TODO Auto-generated constructor stub
}

simple_uniform_buffer::~simple_uniform_buffer() {}

} /* namespace graphics */
} /* namespace mce */
