/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset.hpp>

namespace mce {
namespace asset {

asset::asset(const std::string& name) : current_state_{state::initial}, name_{name}, size_{0} {}

void asset::complete_loading(std::shared_ptr<const char> loaded_data, size_t size) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	this->data_ = loaded_data;
	size_ = size;
	current_state_ = state::ready;
	auto this_shared = this->shared_from_this();
	lock.unlock();
	// From here on the asset object is immutable and can therefore be read without holding a lock
	completed_cv.notify_all();
	for(auto& handler : completion_handlers) { handler(this_shared); }
	completion_handlers.clear();
	completion_handlers.shrink_to_fit();
}

} // namespace asset
} // namespace mce
