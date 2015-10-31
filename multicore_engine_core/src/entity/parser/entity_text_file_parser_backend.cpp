/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/parser/entity_text_file_parser_frontend.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/parser/entity_text_file_parser.hpp>
#include <entity/entity_manager.hpp>

namespace mce {
namespace entity {
namespace parser {

void entity_text_file_parser_backend::ast_definition_visitor::operator()(const ast::entity_definition& node) {
	if(node.super_name.empty()) {
	} else {
	}
}
void entity_text_file_parser_backend::ast_definition_visitor::operator()(ast::include_instruction& node) {
	// TODO Handle relative paths if needed
	node.included_ast = std::make_shared<ast::ast_wrapper>(backend.load_file(node.filename));
	backend.process_entity_definitions(node.included_ast->root);
}
void entity_text_file_parser_backend::ast_definition_visitor::operator()(const ast::entity_instance&) {}

void entity_text_file_parser_backend::ast_instance_visitor::operator()(const ast::entity_definition&) {}
void entity_text_file_parser_backend::ast_instance_visitor::operator()(ast::include_instruction& node) {
	backend.process_entity_definitions(node.included_ast->root);
}
void entity_text_file_parser_backend::ast_instance_visitor::operator()(const ast::entity_instance& node) {
	// TODO: Create entity and give it the specified position and orientation
	if(!node.instance_name.empty()) {
		// TODO: Name entity
	}
}
/*entity_position_t entity_text_file_parser_backend::ast_position_visitor::
operator()(const ast::int_list& node) {}
entity_position_t entity_text_file_parser_backend::ast_position_visitor::
operator()(const ast::float_list& node) {}
entity_position_t entity_text_file_parser_backend::ast_position_visitor::
operator()(const ast::rotation_list& node) {}
entity_position_t entity_text_file_parser_backend::ast_position_visitor::
operator()(const ast::marker_evaluation& node) {}
entity_position_t entity_text_file_parser_backend::ast_position_visitor::
operator()(const ast::entity_reference& node) {}
entity_orientation_t entity_text_file_parser_backend::ast_orientation_visitor::
operator()(const ast::int_list& node) {}
entity_orientation_t entity_text_file_parser_backend::ast_orientation_visitor::
operator()(const ast::float_list& node) {}
entity_orientation_t entity_text_file_parser_backend::ast_orientation_visitor::
operator()(const ast::rotation_list& node) {}
entity_orientation_t entity_text_file_parser_backend::ast_orientation_visitor::
operator()(const ast::marker_evaluation& node) {}
entity_orientation_t entity_text_file_parser_backend::ast_orientation_visitor::
operator()(const ast::entity_reference& node) {}*/

void entity_text_file_parser_backend::process_entity_definitions(ast::ast_root& root_node) {
	ast_definition_visitor visitor{*this};
	for(auto&& element : root_node) element.apply_visitor(visitor);
}
void entity_text_file_parser_backend::process_entity_instances(ast::ast_root& root_node) {
	ast_instance_visitor visitor{*this};
	for(auto&& element : root_node) element.apply_visitor(visitor);
}
ast::ast_root entity_text_file_parser_backend::load_file(const std::string& filename) {
	ast::ast_root root;
	(void)filename;
	// TODO: Implement loading of file when asset manager is available.
	// TODO: Parse loaded file using frontend
	return root;
}
void entity_text_file_parser_backend::load_and_process_file(const std::string& filename) {
	ast::ast_root root = load_file(filename);
	process_entity_definitions(root);
	process_entity_instances(root);
}

} // namespace parser
} // namespace entity
} // namespace mce
