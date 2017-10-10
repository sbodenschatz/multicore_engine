/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_manager.cpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/vector.hpp>
#include <cassert>
#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/entity/entity_configuration.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/entity/parser/entity_template_lang_parser.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/finally.hpp>
#include <tuple>
#include <utility>

namespace mce {
namespace entity {

entity_manager::entity_manager(core::engine* engine) : engine(engine) {
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
void entity_manager::load_entities_from_template_lang_file(const asset::asset_ptr& template_lang_file_asset) {
	parser::entity_template_lang_parser_backend parser_backend(*this);
	parser_backend.load_and_process_file(template_lang_file_asset);
}
entity* entity_manager::create_entity(const entity_configuration* config) {
	assert(!read_only_mode);
	auto id = next_id++;
	auto it = entities.emplace(id, *this);
	if(config) config->create_components(*it);
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
			throw missing_entity_exception("non-existent entity requested for destruction.");
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
		if(count == 0) throw missing_entity_exception("non-existent entity requested for destruction.");
	}
	entities.find_and_erase(*entity);
}

entity* entity_manager::find_entity(long long id) const {
	std::unique_lock<std::mutex> lock(id_map_mutex, std::defer_lock);
	if(!read_only_mode) lock.lock();
	auto it = entity_id_map.find(id);
	if(it != entity_id_map.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}
entity* entity_manager::find_entity(const std::string& name) const {
	std::unique_lock<std::mutex> lock(name_map_mutex, std::defer_lock);
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
const entity_configuration* entity_manager::find_entity_configuration(const std::string& name) const {
	auto it = entity_configurations.find(name);
	if(it != entity_configurations.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}
const abstract_component_type* entity_manager::find_component_type(const std::string& name) const {
	auto it = component_types.find(name);
	if(it != component_types.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}
const abstract_component_type* entity_manager::find_component_type(component_type_id_t id) const {
	auto it = component_types_by_id.find(id);
	if(it != component_types_by_id.end()) {
		return it->second;
	} else {
		return nullptr;
	}
}

void entity_manager::add_entity_configuration(std::unique_ptr<entity_configuration>&& entity_config) {
	bool success = false;
	const auto& name = entity_config->name();
	std::tie(std::ignore, success) = entity_configurations.emplace(name, std::move(entity_config));
}

void entity_manager::store_entities_to_bstream(bstream::obstream& ostr) {
	bool old_readonly_mode = read_only_mode.exchange(true);
	auto finaly_v = util::finally([old_readonly_mode, this]() { read_only_mode.store(old_readonly_mode); });
	ostr << uint64_t(entities.size());
	for(const entity& ent : entities) {
		ostr << ent.id();
		ent.store_to_bstream(ostr);
	}
	std::lock_guard<std::mutex> lock(name_map_mutex);
	ostr << uint64_t(entity_name_map.size());
	for(const auto& name_id_element : entity_name_map) {
		ostr << name_id_element.first;
		ostr << name_id_element.second;
	}
}
void entity_manager::load_entities_from_bstream(bstream::ibstream& istr) {
	uint64_t entity_count;
	istr >> entity_count;
	boost::container::flat_map<entity_id_t, entity_id_t> id_renaming;
	for(uint64_t i = 0; i < entity_count; ++i) {
		entity_id_t read_id;
		istr >> read_id;
		auto entity = find_entity(read_id);
		if(entity) {
			id_renaming[read_id] = read_id;
			entity->load_from_bstream(istr, *this, engine);
		} else {
			entity = create_entity();
			id_renaming[read_id] = entity->id();
			entity->load_from_bstream(istr, *this, engine);
		}
	}
	uint64_t name_count;
	istr >> name_count;
	std::string name;
	entity_id_t read_id;
	for(uint64_t i = 0; i < name_count; ++i) {
		istr >> name;
		istr >> read_id;
		assign_entity_name(name, read_id);
	}
	// Fix entity references:
	for(entity& ent : entities) {
		for(auto& comp_entry : ent.components()) {
			const component_pool_ptr& comp = comp_entry.second;
			auto& props = comp->configuration().type().properties();
			for(auto& abst_prop : props) {
				auto prop = abst_prop->as_type<entity_reference>();
				if(prop) {
					prop->set_value(*comp,
									entity_reference(prop->get_value(*comp).referenced_entity_name(), *this));
				}
			}
		}
	}
}

} // namespace entity
} // namespace mce
