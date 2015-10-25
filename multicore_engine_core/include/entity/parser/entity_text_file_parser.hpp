/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_parser.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_

#include "entity_text_file_ast.hpp"
#include <memory>

namespace mce {
namespace entity {
namespace parser {

struct entity_text_file_grammar;

class entity_text_file_parser_frontend {
	std::unique_ptr<entity_text_file_grammar> grammar;

public:
	entity_text_file_parser_frontend();
	~entity_text_file_parser_frontend();

	void parse(const char* first, const char* last, ast::ast_root& ast_root);
};

} // namespace parser
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_HPP_ */
