/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/polygon_model.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <exception>
#include <mce/asset/asset.hpp>
#include <mce/bstream/asset_ibstream.hpp>
#include <mce/exceptions.hpp>
#include <mce/model/model_data_manager.hpp>
#include <mce/model/polygon_model.hpp>
#include <mce/util/local_function.hpp>
#include <string>

namespace mce {
namespace model {

polygon_model::polygon_model(const std::string& name) : current_state_{state::loading}, name_{name} {}
polygon_model::polygon_model(std::string&& name) : current_state_{state::loading}, name_{std::move(name)} {}

void polygon_model::complete_loading(const asset::asset_ptr& polygon_asset) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	bstream::asset_ibstream stream{polygon_asset};
	try {
		stream >> meta_data_;
	} catch(...) {
		raise_error_flag(std::current_exception());
	}
	if(!stream) {
		raise_error_flag(std::make_exception_ptr(
				io_exception("Error on loading meta data for polygon model '" + name_ + "'.")));
		return;
	}
	content_data_ = std::shared_ptr<const char>(polygon_asset->data_shared(),
												polygon_asset->data() + meta_data_.content_range.begin());
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const polygon_model>(this->shared_from_this());
	lock.unlock();
	// From here on the polygon model object is immutable and can therefore be read without holding a lock
	for(auto& handler : completion_handlers) {
		try {
			handler(this_shared);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	completion_handlers.clear();
	error_handlers.clear();
	completion_handlers.shrink_to_fit();
	error_handlers.shrink_to_fit();
}

} // namespace model
} // namespace mce
