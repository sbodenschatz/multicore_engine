/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_type.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_TYPE_HPP_
#define ENTITY_COMPONENT_TYPE_HPP_

#include <memory>
#include <vector>
#include <string>
#include "ecs_types.hpp"
#include <reflection/property.hpp>
#include "component_type_id_manager.hpp"

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
	typedef std::vector<std::unique_ptr<reflection::abstract_property<component>>> property_list;
	abstract_component_type(const abstract_component_type&) = delete;
	abstract_component_type(abstract_component_type&&) = delete;
	abstract_component_type& operator=(const abstract_component_type&) = delete;
	abstract_component_type& operator=(abstract_component_type&&) = delete;
	virtual ~abstract_component_type() = default;
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) = 0;
	virtual const property_list& properties() const noexcept = 0;

	component_type_id_t id() const noexcept {
		return id_;
	}

	const std::string& name() const noexcept {
		return name_;
	}
};

template <typename T, typename F>
class component_type : public abstract_component_type {
	F factory_function_;
	property_list properties_;

public:
	component_type(const std::string& name, const F& factory_function)
			: abstract_component_type(component_type_id_manager::id<T>(), name),
			  factory_function_(factory_function) {
		T::fill_property_list(properties_);
	}
	virtual ~component_type() override = default;
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) override {
		return factory_function_(owner, config, engine);
	}

	virtual const property_list& properties() const noexcept override {
		return properties_;
	}
};

template <typename T, typename F>
std::unique_ptr<abstract_component_type> make_component_type(const std::string& name,
															 const F& factory_function) {
	return std::make_unique<component_type<T, F>>(name, factory_function);
}

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_TYPE_HPP_ */
