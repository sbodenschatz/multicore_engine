/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_type.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_TYPE_HPP_
#define ENTITY_COMPONENT_TYPE_HPP_

#include <string>
#include "ecs_types.hpp"

namespace mce {
namespace core {
class engine;
} // namespace core

namespace entity {
class component_configuration;
class entity;

class abstract_component_type {
	component_type_id_t id_;
	std::string name_;

protected:
	abstract_component_type(component_type_id_t id, const std::string& name) : id_(id), name_(name) {}

public:
	abstract_component_type(const abstract_component_type&) = delete;
	abstract_component_type(abstract_component_type&&) = delete;
	abstract_component_type& operator=(const abstract_component_type&) = delete;
	abstract_component_type& operator=(abstract_component_type&&) = delete;
	virtual ~abstract_component_type() = default;
	virtual component_pool_ptr create_component(entity& owner, component_configuration& config,
												core::engine& engine) = 0;
};

template <typename T>
class component_type : public abstract_component_type {
	virtual ~component_type() override = default;
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_TYPE_HPP_ */
