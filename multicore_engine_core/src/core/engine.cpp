/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/engine.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset_manager.hpp>
#include <core/engine.hpp>
#include <entity/entity_manager.hpp>

namespace mce {
namespace core {

engine::engine() : asset_manager_{std::make_unique<asset::asset_manager>()} {
	entity_manager_ = std::make_unique<entity::entity_manager>(this);
}

} // namespace core
} // namespace mce
