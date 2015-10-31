/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_parser.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_

#include "../ecs_types.hpp"
#include "entity_text_file_ast.hpp"
#include <memory>

namespace mce {
namespace entity {
class entity_manager;
namespace parser {

struct entity_text_file_grammar;
struct entity_text_file_skipper;

class entity_text_file_parser_frontend {
	std::unique_ptr<entity_text_file_grammar> grammar;
	std::unique_ptr<entity_text_file_skipper> skipper;

public:
	entity_text_file_parser_frontend();
	~entity_text_file_parser_frontend();

	bool parse(const char*& first, const char* last, ast::ast_root& ast_root);
};

class entity_text_file_parser_backend {
	struct ast_definition_visitor : boost::static_visitor<> {
		entity_text_file_parser_backend& backend;
		void operator()(const ast::entity_definition& node);
		void operator()(ast::include_instruction& node);
		void operator()(const ast::entity_instance& node);
		ast_definition_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};
	struct ast_instance_visitor : boost::static_visitor<> {
		entity_text_file_parser_backend& backend;
		void operator()(const ast::entity_definition& node);
		void operator()(ast::include_instruction& node);
		void operator()(const ast::entity_instance& node);
		ast_instance_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};
	struct ast_position_visitor : boost::static_visitor<entity_position_t> {
		entity_position_t operator()(const ast::int_list& node);
		entity_position_t operator()(const ast::float_list& node);
		entity_position_t operator()(const ast::rotation_list& node);
		entity_position_t operator()(const ast::marker_evaluation& node);
		entity_position_t operator()(const ast::entity_reference& node);
	};
	struct ast_orientation_visitor : boost::static_visitor<entity_orientation_t> {
		entity_orientation_t operator()(const ast::int_list& node);
		entity_orientation_t operator()(const ast::float_list& node);
		entity_orientation_t operator()(const ast::rotation_list& node);
		entity_orientation_t operator()(const ast::marker_evaluation& node);
		entity_orientation_t operator()(const ast::entity_reference& node);
	};

	entity_manager& em;

	void process_entity_definitions(ast::ast_root& root_node);
	void process_entity_instances(ast::ast_root& root_node);
	ast::ast_root load_file(const std::string& filename);

public:
	entity_text_file_parser_backend(entity_manager& em) : em(em) {}
	void load_and_process_file(const std::string& filename);
};

} // namespace parser
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_ */
