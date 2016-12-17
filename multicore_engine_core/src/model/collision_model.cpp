/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/collision_model.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <bstream/asset_ibstream.hpp>
#include <model/collision_model.hpp>

namespace mce {
namespace model {

collision_model::collision_model(const std::string& name) : current_state_{state::loading}, name_{name} {}
collision_model::collision_model(std::string&& name)
		: current_state_{state::loading}, name_{std::move(name)} {}

void collision_model::complete_loading(const asset::asset_ptr& collision_asset) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	bstream::asset_ibstream stream{collision_asset};
	stream >> data_;
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const collision_model>(this->shared_from_this());
	lock.unlock();
	// From here on the collision model object is immutable and can therefore be read without holding a lock
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
