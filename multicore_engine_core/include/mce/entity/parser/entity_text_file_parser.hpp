/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/parser/entity_text_file_parser.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_

/**
 * \file
 * Defines the parser class for entity text files.
 */

#include <boost/variant/static_visitor.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/entity/parser/entity_text_file_ast.hpp>
#include <mce/entity/parser/entity_text_file_parser_frontend.hpp>
#include <memory>
#include <string>

namespace mce {
namespace entity {
class entity_manager;
namespace parser {

/// Implements the back-end of the entity text file parser.
/**
 * Processes the AST and feeds the entity type definitions and instantiations into a given entity_manager.
 */
class entity_text_file_parser_backend {
	friend class entity_manager;
	struct ast_definition_visitor : boost::static_visitor<> {
		entity_text_file_parser_backend& backend;
		void operator()(const ast::entity_definition& node);
		void operator()(ast::include_instruction& node);
		void operator()(const ast::entity_instance& node);
		explicit ast_definition_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};
	struct ast_instance_visitor : boost::static_visitor<> {
		entity_text_file_parser_backend& backend;
		void operator()(const ast::entity_definition& node);
		void operator()(ast::include_instruction& node);
		void operator()(const ast::entity_instance& node);
		explicit ast_instance_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};
	struct ast_position_visitor : boost::static_visitor<entity_position_t> {
		entity_text_file_parser_backend& backend;
		entity_position_t operator()(const ast::int_list& node);
		entity_position_t operator()(const ast::float_list& node);
		entity_position_t operator()(const ast::rotation_list& node);
		entity_position_t operator()(const ast::marker_evaluation& node);
		entity_position_t operator()(const ast::entity_reference& node);
		explicit ast_position_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};
	struct ast_orientation_visitor : boost::static_visitor<entity_orientation_t> {
		entity_text_file_parser_backend& backend;
		entity_orientation_t operator()(const ast::int_list& node);
		entity_orientation_t operator()(const ast::float_list& node);
		entity_orientation_t operator()(const ast::rotation_list& node);
		entity_orientation_t operator()(const ast::marker_evaluation& node);
		entity_orientation_t operator()(const ast::entity_reference& node);
		explicit ast_orientation_visitor(entity_text_file_parser_backend& backend) : backend(backend) {}
	};

	entity_manager& em;

	void process_entity_definitions(ast::ast_root& root_node);
	void process_entity_instances(ast::ast_root& root_node);
	ast::ast_root load_file(const asset::asset_ptr& text_file_asset);

public:
	/// Constructs a parser back-end for the given entity_manager.
	explicit entity_text_file_parser_backend(entity_manager& em) : em(em) {}
	/// Loads and processes the file represented by the given asset.
	void load_and_process_file(const asset::asset_ptr& text_file_asset);
};

} // namespace parser
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_ */
