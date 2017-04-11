/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/engine.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <core/engine.hpp>

namespace mce {
namespace core {

engine::engine() {
	entity_manager_ = std::make_unique<entity::entity_manager>(this);
}

} // namespace core
} // namespace mce
