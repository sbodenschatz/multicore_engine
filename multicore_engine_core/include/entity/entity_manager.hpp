/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_manager.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_MANAGER_HPP_
#define ENTITY_ENTITY_MANAGER_HPP_

#include <string>
#include <memory>
#include <boost/container/flat_map.hpp>
#include <containers/unordered_object_pool.hpp>
#include "ecs_types.hpp"
#include "entity.hpp"
#include "component_type.hpp"

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
class entity_configuration;
class abstract_component_type;

class entity_manager {
	core::engine& engine;
	containers::unordered_object_pool<entity> entities;
	boost::container::flat_map<entity_id_t, entity*> entity_id_map;
	boost::container::flat_map<std::string, entity_id_t> entity_name_map;
	boost::container::flat_map<std::string, std::unique_ptr<entity_configuration>> entity_configurations;
	boost::container::flat_map<std::string, std::unique_ptr<abstract_component_type>> component_types;

	void register_builtin_components();

public:
	entity_manager(core::engine& engine);
	entity_manager(const entity_manager&) = delete;
	entity_manager(entity_manager&&) = delete;
	entity_manager& operator=(const entity_manager&) = delete;
	entity_manager& operator=(entity_manager&&) = delete;
	~entity_manager();

	template <typename T, typename F>
	void register_component_type(const std::string& name, const F& factory_function) {
		bool success = false;
		std::tie(std::ignore, success) =
				component_types.emplace(name, make_component_type<T>(name, factory_function));
		if(!success) throw std::logic_error("Duplicate component type name.");
	}
};

} // namespace entity
} // namespace mce

#define REGISTER_COMPONENT_TYPE(ENTITYMANAGER, TYPE, NAME, FACTORYEXPR)                                      \
	ENTITYMANAGER.register_component_type<TYPE>(NAME, [](auto&& owner, auto&& config, auto&& engine) {       \
		/*Silence unused parameter warnings:*/                                                               \
		static_cast<void>(owner), static_cast<void>(config), static_cast<void>(engine);                      \
		return FACTORYEXPR;                                                                                  \
	})

#define REGISTER_COMPONENT_TYPE_SIMPLE(ENTITYMANAGER, NAME, FACTORYEXPR)                                     \
	ENTITYMANAGER.register_component_type<NAME##_component>(                                                 \
			#NAME, [](auto&& owner, auto&& config, auto&& engine) {                                          \
				/*Silence unused parameter warnings:*/                                                       \
				static_cast<void>(owner), static_cast<void>(config), static_cast<void>(engine);              \
				return FACTORYEXPR;                                                                          \
			})

#endif /* ENTITY_ENTITY_MANAGER_HPP_ */
