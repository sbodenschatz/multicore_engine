/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/engine.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CORE_ENGINE_HPP_
#define CORE_ENGINE_HPP_

#include <cassert>
#include <entity/entity_manager.hpp>
#include <memory>

namespace mce {
namespace core {

class engine {
	std::unique_ptr<entity::entity_manager> entity_manager_;

public:
	engine();

	const entity::entity_manager& entity_manager() const {
		assert(entity_manager_);
		return *entity_manager_;
	}
	entity::entity_manager& entity_manager() {
		assert(entity_manager_);
		return *entity_manager_;
	}
};

} // namespace core
} // namespace mce

#endif /* CORE_ENGINE_HPP_ */
