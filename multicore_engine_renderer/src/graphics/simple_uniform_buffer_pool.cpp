/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/simple_uniform_buffer_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/simple_uniform_buffer_pool.hpp>

namespace mce {
namespace graphics {

simple_uniform_buffer_pool::simple_uniform_buffer_pool(device& dev, device_memory_manager_interface& mem_mgr,
													   destruction_queue_manager* destruction_manager,
													   vk::DeviceSize buffer_size)
		: dev_{&dev}, mem_mgr_{&mem_mgr}, dqm_{destruction_manager}, buffer_size_{buffer_size} {
	buffers_.emplace_back(*dev_, *mem_mgr_, dqm_, buffer_size_);
}

} /* namespace graphics */
} /* namespace mce */
