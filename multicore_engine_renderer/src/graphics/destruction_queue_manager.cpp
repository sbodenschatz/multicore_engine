/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/destruction_queue.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

destruction_queue_manager::~destruction_queue_manager() {
	try {
		dev_.native_device().waitIdle();
	} catch(...) {
	}
	// TODO: Ordering
	queues.clear();
}

void destruction_queue_manager::cleanup_and_set_current(uint32_t ring_index) {
	std::lock_guard<std::mutex> lock(queue_mutex);
	current_ring_index = ring_index;
	/// TODO: Ordering
	queues[current_ring_index].clear();
}

} /* namespace graphics */
} /* namespace mce */
