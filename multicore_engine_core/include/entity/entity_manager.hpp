/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_manager.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_MANAGER_HPP_
#define ENTITY_ENTITY_MANAGER_HPP_

/**
 * \file
 * Definition of the entity_manager class.
 */

#include <asset/asset_defs.hpp>
#include <boost/container/flat_map.hpp>
#include <containers/unordered_object_pool.hpp>
#include <entity/component_type.hpp>
#include <entity/ecs_types.hpp>
#include <entity/entity.hpp>
#include <atomic>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
namespace parser {
class entity_text_file_parser_backend;
} // namespace parser
class entity_configuration;
class abstract_component_type;

/// Manages the entities in a scene and the available component types.
class entity_manager {
	core::engine* engine;
	std::atomic<entity_id_t> next_id{1};
	containers::unordered_object_pool<entity> entities;
	// TODO: Check if this can be non-atomic:
	std::atomic<bool> read_only_mode{false};
	mutable std::mutex id_map_mutex;
	boost::container::flat_map<entity_id_t, containers::unordered_object_pool<entity>::iterator>
			entity_id_map;
	mutable std::mutex name_map_mutex;
	boost::container::flat_map<std::string, entity_id_t> entity_name_map;
	// The following members may only be written to in strictly single-threaded access:
	boost::container::flat_map<std::string, std::unique_ptr<entity_configuration>> entity_configurations;
	boost::container::flat_map<std::string, std::unique_ptr<abstract_component_type>> component_types;
	boost::container::flat_map<component_type_id_t, abstract_component_type*> component_types_by_id;

	void register_builtin_components();

public:
	friend class mce::entity::parser::entity_text_file_parser_backend;
	/// Constructs an entity_manager for the given engine object.
	explicit entity_manager(core::engine* engine);
	/// Forbids copy-construction for entity_manager.
	entity_manager(const entity_manager&) = delete;
	/// Forbids move-construction for entity_manager.
	entity_manager(entity_manager&&) = delete;
	/// Forbids copy-assignment for entity_manager.
	entity_manager& operator=(const entity_manager&) = delete;
	/// Forbids move-assignment for entity_manager.
	entity_manager& operator=(entity_manager&&) = delete;
	/// Destroys the entity_manager.
	~entity_manager();

	/// Deletes all entity objects from the manager.
	void clear_entities();
	/// Deletes all entity objects and entity_configuration objects from the manager.
	void clear_entities_and_entity_configurations();
	/// Loads entities and entity configurations from the entity text file represented by the given asset.
	void load_entities_from_text_file(const asset::asset_ptr& text_file_asset);
	/// Adds an entity_configuration to the manager.
	void add_entity_configuration(std::unique_ptr<entity_configuration>&& entity_config);
	/// Creates an entity from the referenced entity_configuration.
	entity* create_entity(const entity_configuration* config = nullptr);
	/// Destroys the entity with the given id.
	void destroy_entity(entity_id_t id);
	/// Destroys the referenced entity.
	void destroy_entity(entity* entity);

	/// Returns a pointer to the entity with the given id or nullptr if no such entity exists.
	entity* find_entity(long long id) const;
	/// Returns a pointer to the entity with the given name or nullptr if no such entity exists.
	entity* find_entity(const std::string& name) const;
	/// Assigns the given entity name to the given entity id.
	void assign_entity_name(const std::string& name, long long id);
	/// \brief Returns a pointer to the entity_configuration with the given name or nullptr if no such
	/// entity_configuration exists.
	const entity_configuration* find_entity_configuration(const std::string& name) const;
	/// \brief Returns a pointer to the abstract_component_type with the given name or nullptr if no such
	/// abstract_component_type exists.
	const abstract_component_type* find_component_type(const std::string& name) const;
	/// \brief Returns a pointer to the abstract_component_type with the given type id or nullptr if no such
	/// abstract_component_type exists.
	const abstract_component_type* find_component_type(component_type_id_t id) const;

	/// \brief Stores the current state of the entities (position, orientation, attached components and their
	/// property values) to the given bstream.
	/**
	 * May only be called if no other threads manipulate the stored entity data concurrently and the set of
	 * entities.
	 * Therefore this operation transitions the entity_manager to read-only mode.
	 */
	void store_entities_to_bstream(bstream::obstream& ostr);
	/// Loads the state of the entities (as stored by store_to_bstream) from the given bstream.
	/**
	 * May only be called if no other threads manipulate the stored entity data concurrently and the set of
	 * entities.
	 */
	void load_entities_from_bstream(bstream::ibstream& istr);

	/// Registers a component type with the given name and factory function.
	template <typename T, typename F>
	void register_component_type(const std::string& name, const F& factory_function) {
		bool success = false;
		decltype(component_types)::iterator it;
		std::tie(it, success) =
				component_types.emplace(name, make_component_type<T>(engine, name, factory_function));
		if(!success) throw std::logic_error("Duplicate component type name.");
		component_types_by_id.emplace(it->second->id(), it->second.get());
	}
};

} // namespace entity
} // namespace mce

/// Simplifies the registration of a component type by reducing boilerplate.
#define REGISTER_COMPONENT_TYPE(ENTITYMANAGER, TYPE, NAME, FACTORYEXPR, CAPTURE)                             \
	ENTITYMANAGER.register_component_type<TYPE>(NAME,                                                        \
												[CAPTURE](auto&& owner, auto&& config, auto&& engine) {      \
		UNUSED(owner), UNUSED(config), UNUSED(engine);                                                       \
		return FACTORYEXPR;                                                                                  \
												})

/// \brief Simplifies the registration of a component type by applying the convention, that component classes
/// are named [component type name]_component.
#define REGISTER_COMPONENT_TYPE_SIMPLE(ENTITYMANAGER, NAME, FACTORYEXPR, CAPTURE)                            \
	ENTITYMANAGER.register_component_type<NAME##_component>(                                                 \
			#NAME, [CAPTURE](auto&& owner, auto&& config, auto&& engine) {                                   \
		UNUSED(owner), UNUSED(config), UNUSED(engine);                                                       \
		return FACTORYEXPR;                                                                                  \
			})

#endif /* ENTITY_ENTITY_MANAGER_HPP_ */
