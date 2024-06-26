/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/parser/entity_template_lang_parser_frontend.cpp
 * Copyright 2015-2018 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/variant/variant.hpp>
#include <glm/glm.hpp>
#include <iterator>
#include <mce/asset/asset.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/asset/asset_manager.hpp>
#include <mce/core/engine.hpp>
#include <mce/entity/component_configuration.hpp>
#include <mce/entity/component_type.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/entity/entity.hpp>
#include <mce/entity/entity_configuration.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/entity/parser/entity_template_lang_ast.hpp>
#include <mce/entity/parser/entity_template_lang_parser.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/unused.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace mce {
namespace entity {
namespace parser {

void entity_template_lang_parser_backend::ast_definition_visitor::
operator()(const ast::entity_definition& node) {
	std::unique_ptr<entity_configuration> config;
	if(!node.super_name.empty()) {
		auto super_config = backend.em.find_entity_configuration(node.super_name);
		if(!super_config)
			throw missing_entity_config_exception("Super entity configuration '" + node.super_name +
												  "' not found.");
		config = std::make_unique<entity_configuration>(*super_config);
		config->name(node.name);
	} else {
		config = std::make_unique<entity_configuration>(node.name);
	}
	auto& comp_configs = config->components();
	for(const auto& comp_def : node.components) {
		auto comp_conf_it = std::find_if(comp_configs.begin(), comp_configs.end(), [&](const auto& elem) {
			return elem->type().name() == comp_def.name;
		});
		if(comp_conf_it == comp_configs.end()) {
			auto comp_type = backend.em.find_component_type(comp_def.name);
			if(!comp_type)
				throw invalid_component_type_exception("Unknown component type '" + comp_def.name + "'.");
			comp_conf_it = comp_configs.emplace(comp_configs.end(), std::make_unique<component_configuration>(
																			backend.em.engine, *comp_type));
		} else if(comp_def.replace) {
			auto comp_type = backend.em.find_component_type(comp_def.name);
			if(!comp_type)
				throw invalid_component_type_exception("Unknown component type '" + comp_def.name + "'.");
			*comp_conf_it = std::make_unique<component_configuration>(backend.em.engine, *comp_type);
		}
		auto& comp_conf = **comp_conf_it;
		for(const auto& var_entry : comp_def.variables) {
			comp_conf.make_assignment(var_entry.name, var_entry.value, node.name, backend.em);
		}
	}
	backend.em.add_entity_configuration(std::move(config));
}
void entity_template_lang_parser_backend::ast_definition_visitor::operator()(ast::include_instruction& node) {
	if(!backend.em.engine)
		throw invalid_operation_exception("Can't include files because a null-engine is given and therefore "
										  "no asset_loader is available");
	// TODO Handle relative paths if needed
	auto included = backend.em.engine->asset_manager().load_asset_sync(node.filename);
	node.included_ast = std::make_shared<ast::ast_wrapper>(backend.load_file(included));
	backend.process_entity_definitions(node.included_ast->root);
}
void entity_template_lang_parser_backend::ast_definition_visitor::operator()(const ast::entity_instance&) {}

void entity_template_lang_parser_backend::ast_instance_visitor::operator()(const ast::entity_definition&) {}
void entity_template_lang_parser_backend::ast_instance_visitor::operator()(ast::include_instruction& node) {
	backend.process_entity_instances(node.included_ast->root);
}
void entity_template_lang_parser_backend::ast_instance_visitor::operator()(const ast::entity_instance& node) {
	// Create entity and give it the specified position and orientation
	auto entity_conf = backend.em.find_entity_configuration(node.type_name);
	if(!entity_conf)
		throw missing_entity_config_exception("Unknown entity configuration '" + node.type_name + "'.");
	auto entity = backend.em.create_entity(entity_conf);
	ast_position_visitor pos_visitor(backend);
	entity->position(node.position_parameter.apply_visitor(pos_visitor));
	ast_orientation_visitor orientation_visitor(backend);
	entity->orientation(node.orientation_parameter.apply_visitor(orientation_visitor));
	if(!node.instance_name.empty()) backend.em.assign_entity_name(node.instance_name, entity->id());
}
entity_position_t entity_template_lang_parser_backend::ast_position_visitor::
operator()(const ast::int_list& node) {
	entity_position_t pos{0.0f};
	for(unsigned int i = 0; i < pos.length() && i < node.size(); ++i) {
		pos[i] = float(node[i]);
	}
	return pos;
}
entity_position_t entity_template_lang_parser_backend::ast_position_visitor::
operator()(const ast::float_list& node) {
	entity_position_t pos{0.0f};
	for(unsigned int i = 0; i < pos.length() && i < node.size(); ++i) {
		pos[i] = node[i];
	}
	return pos;
}
entity_position_t entity_template_lang_parser_backend::ast_position_visitor::
operator()(const ast::rotation_list&) {
	throw value_type_exception("Rotation list given as position.");
}
entity_position_t entity_template_lang_parser_backend::ast_position_visitor::
operator()(const ast::marker_evaluation& node) {
	UNUSED(node);
	throw unimplemented_exception("not implemented");
}
entity_position_t entity_template_lang_parser_backend::ast_position_visitor::
operator()(const ast::entity_reference& node) {
	auto ent = backend.em.find_entity(node.referred_name);
	if(ent) {
		return ent->position();
	} else {
		throw missing_entity_exception("Named entity not found.");
	}
}
entity_orientation_t entity_template_lang_parser_backend::ast_orientation_visitor::
operator()(const ast::int_list& node) {
	if(node.empty()) return entity_orientation_t{1.0f, 0.0f, 0.0f, 0.0f};
	if(node.size() < 4) throw value_type_exception("Invalid angle-axis quaternion literal.");
	return glm::angleAxis(glm::radians(float(node[0])),
						  glm::vec3(float(node[1]), float(node[2]), float(node[3])));
}
entity_orientation_t entity_template_lang_parser_backend::ast_orientation_visitor::
operator()(const ast::float_list& node) {
	if(node.empty()) return entity_orientation_t{1.0f, 0.0f, 0.0f, 0.0f};
	if(node.size() < 4) throw value_type_exception("Invalid angle-axis quaternion literal.");
	return glm::angleAxis(glm::radians(float(node[0])), glm::vec3(node[1], node[2], node[3]));
}
entity_orientation_t entity_template_lang_parser_backend::ast_orientation_visitor::
operator()(const ast::rotation_list& node) {
	entity_orientation_t orientation{1.0f, 0.0f, 0.0f, 0.0f};
	for(auto&& entry : node) {
		glm::vec3 axis{0.0f, 0.0f, 0.0f};
		switch(entry.axis) {
		case ast::rotation_axis::x: axis.x = 1.0f; break;
		case ast::rotation_axis::y: axis.y = 1.0f; break;
		case ast::rotation_axis::z: axis.z = 1.0f; break;
		default: throw value_type_exception("Invalid rotation axis in AST."); break;
		}
		orientation = orientation * glm::angleAxis(glm::radians(entry.angle), axis);
	}
	return orientation;
}
entity_orientation_t entity_template_lang_parser_backend::ast_orientation_visitor::
operator()(const ast::marker_evaluation& node) {
	UNUSED(node);
	throw unimplemented_exception("not implemented");
}
entity_orientation_t entity_template_lang_parser_backend::ast_orientation_visitor::
operator()(const ast::entity_reference& node) {
	auto ent = backend.em.find_entity(node.referred_name);
	if(ent) {
		return ent->orientation();
	} else {
		throw missing_entity_exception("Named entity not found.");
	}
}

void entity_template_lang_parser_backend::process_entity_definitions(ast::ast_root& root_node) {
	ast_definition_visitor visitor{*this};
	for(auto&& element : root_node) element.apply_visitor(visitor);
}
void entity_template_lang_parser_backend::process_entity_instances(ast::ast_root& root_node) {
	ast_instance_visitor visitor{*this};
	for(auto&& element : root_node) element.apply_visitor(visitor);
}
ast::ast_root
entity_template_lang_parser_backend::load_file(const asset::asset_ptr& template_lang_file_asset) {
	const char* start = template_lang_file_asset->data();
	const char* end = template_lang_file_asset->data() + template_lang_file_asset->size();
	parser::entity_template_lang_parser_frontend fe;
	auto root = fe.parse(template_lang_file_asset->name(), start, end);
	if(start != end) throw syntax_exception("Partial parse of '" + template_lang_file_asset->name() + "'.");
	return root;
}
void entity_template_lang_parser_backend::load_and_process_file(
		const asset::asset_ptr& template_lang_file_asset) {
	ast::ast_root root = load_file(template_lang_file_asset);
	process_entity_definitions(root);
	process_entity_instances(root);
}

} // namespace parser
} // namespace entity
} // namespace mce
