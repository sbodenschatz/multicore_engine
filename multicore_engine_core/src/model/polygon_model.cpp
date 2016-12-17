/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/model.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <asset/asset_manager.hpp>
#include <bstream/asset_ibstream.hpp>
#include <model/model_manager.hpp>
#include <model/polygon_model.hpp>

namespace mce {
namespace model {

polygon_model::polygon_model(const std::string& name) : current_state_{state::loading}, name_{name} {}
polygon_model::polygon_model(std::string&& name) : current_state_{state::loading}, name_{std::move(name)} {}

void polygon_model::complete_loading(const asset::asset_ptr& polygon_asset, model_manager& mm) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	bstream::asset_ibstream stream{polygon_asset};
	stream >> meta_data_;
	if(!stream) {
		raise_error_flag(std::make_exception_ptr(
				std::runtime_error("Error on loading meta data for polygon model '" + name_ + "'.")));
		return;
	}
	current_state_ = state::staging;
	lock.unlock();
	mm.start_stage_polygon_model(this->shared_from_this());
}

void polygon_model::complete_staging(model_manager&) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const polygon_model>(this->shared_from_this());
	lock.unlock();
	// From here on the polygon model object is immutable and can therefore be read without holding a lock
	for(auto& handler : completion_handlers) {
		handler(this_shared);
	}
	completion_handlers.clear();
	error_handlers.clear();
	completion_handlers.shrink_to_fit();
	error_handlers.shrink_to_fit();
}

} // namespace model
} // namespace mce
