/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <asset/asset.hpp>

namespace mce {
namespace asset {

asset::asset(const std::string& name) : current_state_{state::initial}, name_{name}, size_{0} {}
asset::asset(std::string&& name) : current_state_{state::initial}, name_{std::move(name)}, size_{0} {}

void asset::complete_loading(const std::shared_ptr<const char>& loaded_data, size_t size) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	this->data_ = loaded_data;
	size_ = size;
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const asset>(this->shared_from_this());
	lock.unlock();
	// From here on the asset object is immutable and can therefore be read without holding a lock
	completed_cv.notify_all();
	for(auto& handler : completion_handlers) {
		try {
			handler(this_shared);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	error_handlers.clear();
	completion_handlers.clear();
	error_handlers.shrink_to_fit();
	completion_handlers.shrink_to_fit();
}

} // namespace asset
} // namespace mce
