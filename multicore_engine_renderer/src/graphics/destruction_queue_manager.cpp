/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/destruction_queue_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/destruction_queue_manager.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

destruction_queue_manager::~destruction_queue_manager() {
	try {
		if(dev_) dev_->native_device().waitIdle();
	} catch(...) {
	}
	reset_visitor v;
	/// Ensure destruction in queue order.
	for(uint32_t i = (current_ring_index + 1) % ring_slots; i != current_ring_index;
		i = (i + 1) % ring_slots) {
		for(auto& e : queues[i]) {
			e.apply_visitor(v);
		}
	}
	for(auto& e : queues[current_ring_index]) {
		e.apply_visitor(v);
	}
	queues.clear();
}

void destruction_queue_manager::cleanup_and_set_current(uint32_t ring_index) {
	std::lock_guard<std::mutex> lock(queue_mutex);
	current_ring_index = ring_index;
	reset_visitor v;
	for(auto& e : queues[current_ring_index]) {
		e.apply_visitor(v);
	}
	queues[current_ring_index].clear();
}

} /* namespace graphics */
} /* namespace mce */
