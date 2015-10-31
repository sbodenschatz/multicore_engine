/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_manager.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/entity_manager.hpp>
#include <entity/entity_configuration.hpp>
#include <entity/component_type.hpp>
#include <entity/parser/entity_text_file_parser.hpp>

namespace mce {
namespace entity {

entity_manager::entity_manager(core::engine& engine) : engine(engine) {
	register_builtin_components();
}

entity_manager::~entity_manager() {}

void entity_manager::register_builtin_components() {}

void entity_manager::clear_entities() {
	entities.clear();
	entity_id_map.clear();
	entity_name_map.clear();
}
void entity_manager::clear_entities_and_entity_configurations() {
	clear_entities();
	entity_configurations.clear();
}
void entity_manager::load_entities_from_text_file(const std::string& filename) {
	parser::entity_text_file_parser_backend parser_backend(*this);
	parser_backend.load_and_process_file(filename);
}
entity* entity_manager::create_entity(const entity_configuration& config) {
	assert(!read_only_mode);
	auto id = next_id++;
	auto it = entities.emplace(id);
	config.create_components(*it, engine);
	std::lock_guard<std::mutex> lock(id_map_mutex);
	entity_id_map.insert(std::make_pair(id, it));
	return it;
}

void entity_manager::destroy_entity(entity_id_t id) {
	assert(!read_only_mode);
	containers::unordered_object_pool<entity>::iterator ent_it;
	{
		std::lock_guard<std::mutex> lock(id_map_mutex);
		auto it = entity_id_map.find(id);
		if(it == entity_id_map.end())
			throw std::runtime_error("non-existent entity requested for destruction.");
		ent_it = it->second;
		entity_id_map.erase(it);
	}
	entities.erase(ent_it);
}
void entity_manager::destroy_entity(entity* entity) {
	assert(!read_only_mode);
	{
		std::lock_guard<std::mutex> lock(id_map_mutex);
		auto count = entity_id_map.erase(entity->id());
		if(count == 0) throw std::runtime_error("non-existent entity requested for destruction.");
	}
	entities.find_and_erase(*entity);
}

entity* entity_manager::find_entity(long long id) const {
	std::unique_lock<std::mutex> lock(id_map_mutex);
	if(!read_only_mode) lock.lock();
	auto it = entity_id_map.find(id);
	if(it != entity_id_map.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}
entity* entity_manager::find_entity(const std::string& name) const {
	std::unique_lock<std::mutex> lock(name_map_mutex);
	if(!read_only_mode) lock.lock();
	auto it = entity_name_map.find(name);
	if(it != entity_name_map.end()) {
		return find_entity(it->second);
	} else {
		return nullptr;
	}
}
void entity_manager::assign_entity_name(const std::string& name, long long id) {
	assert(!read_only_mode);
	std::lock_guard<std::mutex> lock(name_map_mutex);
	entity_name_map[name] = id;
}
entity_configuration* entity_manager::find_entity_configuration(const std::string& name) const {
	auto it = entity_configurations.find(name);
	if(it != entity_configurations.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}

} // namespace entity
} // namespace mce
