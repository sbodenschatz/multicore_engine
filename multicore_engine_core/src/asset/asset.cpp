/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset.hpp>

namespace mce {
namespace asset {

asset::asset(const std::string& name) : loaded{false}, error_flag{false}, name{name} {}

void asset::complete_loading(std::shared_ptr<const char> loaded_data) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	this->data = loaded_data;
	loaded = true;
	completed_cv.notify_all();
	lock.unlock();
	auto this_shared = this->shared_from_this();
	for(auto& handler : completion_handlers) { handler(this_shared); }
}

} // namespace asset
} // namespace mce
