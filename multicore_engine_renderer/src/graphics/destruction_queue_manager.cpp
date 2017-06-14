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
	/// Ensure destruction in queue order.
	for(uint32_t i = (current_ring_index + 1) % ring_slots; i != current_ring_index;
		i = (i + 1) % ring_slots) {
		for(auto& e : queues[i]) {
			e.reset();
		}
	}
	for(auto& e : queues[current_ring_index]) {
		e.reset();
	}
	queues.clear();
}

void destruction_queue_manager::cleanup_and_set_current(uint32_t ring_index) {
	auto temp = temp_pool.get();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		in_cleanup_cv.wait(lock, [this]() { return !in_cleanup; });
		in_cleanup = true;
		current_ring_index = ring_index;
		using std::swap;
		swap(*temp, queues[current_ring_index]);
	}
	for(auto& e : *temp) {
		e.reset();
	}
	{
		std::lock_guard<std::mutex> lock(queue_mutex);
		in_cleanup = false;
	}
	in_cleanup_cv.notify_one();
}

void destruction_queue_manager::advance() {
	auto temp = temp_pool.get();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		in_cleanup_cv.wait(lock, [this]() { return !in_cleanup; });
		in_cleanup = true;
		current_ring_index = (current_ring_index + 1) % ring_slots;
		using std::swap;
		swap(*temp, queues[current_ring_index]);
	}
	for(auto& e : *temp) {
		e.reset();
	}
	{
		std::lock_guard<std::mutex> lock(queue_mutex);
		in_cleanup = false;
	}
	in_cleanup_cv.notify_one();
}

} /* namespace graphics */
} /* namespace mce */
