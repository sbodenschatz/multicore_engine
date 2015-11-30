/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/load_unit.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/load_unit.hpp>
#include <bstream/buffer_ibstream.hpp>

namespace mce {
namespace asset {

load_unit::load_unit(const std::string& name) : current_state_{state::initial}, name_{name}, size_{0} {}
load_unit::load_unit(std::string&& name) : current_state_{state::initial}, name_{std::move(name)}, size_{0} {}

void load_unit::load_meta_data(std::shared_ptr<const char> data, size_t size) {
	bstream::buffer_ibstream stream(data.get(), size);
	std::unique_lock<std::mutex> lock(modification_mutex);
	stream >> meta_data_;
	if(!stream) {
		raise_error_flag();
		check_error_flag();
	} else {
		current_state_ = state::meta_ready;
		lock.unlock();
		completed_cv.notify_all();
	}
}
void load_unit::complete_loading(std::shared_ptr<const char> data, size_t size) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	this->payload_data_ = data;
	size_ = size;
	current_state_ = state::data_ready;
	auto this_shared = this->shared_from_this();
	lock.unlock();
	// From here on the load_unit object is immutable and can therefore be read without holding a lock
	completed_cv.notify_all();
	for(auto& handler : completion_handlers) { handler(this_shared); }
	completion_handlers.clear();
	completion_handlers.shrink_to_fit();
}

} // namespace asset
} // namespace mce
