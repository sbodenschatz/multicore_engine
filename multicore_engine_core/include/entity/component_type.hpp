/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_type.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_TYPE_HPP_
#define ENTITY_COMPONENT_TYPE_HPP_

#include "component_type_id_manager.hpp"
#include "ecs_types.hpp"
#include <memory>
#include <reflection/property.hpp>
#include <string>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core

namespace entity {
class component_configuration;
class entity;

class abstract_component_type {
public:
	typedef std::vector<std::unique_ptr<
			reflection::abstract_property<component, abstract_component_property_assignment, core::engine&>>>
			property_list;

private:
	component_type_id_t id_;
	std::string name_;

protected:
	property_list properties_;
	abstract_component_type(component_type_id_t id, const std::string& name) : id_(id), name_(name) {}

public:
	abstract_component_type(const abstract_component_type&) = delete;
	abstract_component_type(abstract_component_type&&) = delete;
	abstract_component_type& operator=(const abstract_component_type&) = delete;
	abstract_component_type& operator=(abstract_component_type&&) = delete;
	virtual ~abstract_component_type() = default;
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) const = 0;
	const property_list& properties() const noexcept {
		return properties_;
	}

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

public:
	component_type(const std::string& name, const F& factory_function)
			: abstract_component_type(component_type_id_manager::id<T>(), name),
			  factory_function_(factory_function) {
		T::fill_property_list(properties_);
	}
	virtual ~component_type() override = default;
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) const override {
		return factory_function_(owner, config, engine);
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
