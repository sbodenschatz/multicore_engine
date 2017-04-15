/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/engine.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef CORE_ENGINE_HPP_
#define CORE_ENGINE_HPP_

#include <cassert>
#include <memory>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset
namespace entity {
class entity_manager;
} // namespace entity

namespace core {

/// Represents the central management class for the subsystems of the engine.
class engine {
	std::unique_ptr<entity::entity_manager> entity_manager_;
	std::unique_ptr<asset::asset_manager> asset_manager_;

public:
	/// Constructs the engine.
	engine();

	/// Allows access to the entity_manager.
	const entity::entity_manager& entity_manager() const {
		assert(entity_manager_);
		return *entity_manager_;
	}
	/// Allows access to the entity_manager.
	entity::entity_manager& entity_manager() {
		assert(entity_manager_);
		return *entity_manager_;
	}
	/// Allows access to the asset_manager.
	const asset::asset_manager& asset_manager() const {
		assert(asset_manager_);
		return *asset_manager_;
	}
	/// Allows access to the asset_manager.
	asset::asset_manager& asset_manager() {
		assert(asset_manager_);
		return *asset_manager_;
	}
};

} // namespace core
} // namespace mce

#endif /* CORE_ENGINE_HPP_ */
