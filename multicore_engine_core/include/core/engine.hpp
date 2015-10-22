/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/engine.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CORE_ENGINE_HPP_
#define CORE_ENGINE_HPP_

#include <memory>
#include <entity/entity_manager.hpp>

namespace mce {
namespace core {

class engine {
	std::unique_ptr<entity::entity_manager> entity_manager_;

public:
	engine();
};

} // namespace core
} // namespace mce

#endif /* CORE_ENGINE_HPP_ */
