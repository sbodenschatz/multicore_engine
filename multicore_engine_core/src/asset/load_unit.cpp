/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/load_unit.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/load_unit.hpp>
#include <bstream/buffer_ibstream.hpp>
#include <algorithm>
#include <stdexcept>

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
	for(auto& handler : completion_handlers) {
		handler(this_shared);
	}
	for(auto& handler : simple_completion_handlers) {
		handler();
	}
	completion_handlers.clear();
	simple_completion_handlers.clear();
	completion_handlers.shrink_to_fit();
	simple_completion_handlers.shrink_to_fit();
}

load_unit::asset_resolution_cookie load_unit::resolve_asset(const std::string& name) const {
	if(!meta_data_ready()) throw std::runtime_error("Load unit meta data not ready yet.");
	auto it = std::find_if(meta_data_.assets.begin(), meta_data_.assets.end(),
						   [&](const asset_meta_data& element) { return name == element.name; });
	if(it != meta_data_.assets.end()) {
		return asset_resolution_cookie(this, it->offset, it->size);
	} else {
		return asset_resolution_cookie();
	}
}

std::pair<std::shared_ptr<const char>, size_t>
load_unit::get_asset_content(const load_unit::asset_resolution_cookie& resolution_cookie) const {
	if(!ready()) throw std::runtime_error("Load unit not ready yet.");
	if(resolution_cookie.load_unit != this)
		throw std::runtime_error("Invalid asset resolution cookie provided (not from this load unit).");
	size_t offset = size_t(resolution_cookie.offset);
	if(uint64_t(offset) != resolution_cookie.offset)
		throw std::runtime_error("Asset offset too big for address space.");
	size_t size = size_t(resolution_cookie.size);
	if(uint64_t(size) != resolution_cookie.size)
		throw std::runtime_error("Asset size too big for address space.");
	return std::make_pair(std::shared_ptr<const char>(payload_data_, payload_data_.get() + offset), size);
}

} // namespace asset
} // namespace mce