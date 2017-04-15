/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/parser/entity_text_file_parser_frontend.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_FRONTEND_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_FRONTEND_HPP_

#include <entity/parser/entity_text_file_ast.hpp>
#include <memory>
#include <string>

namespace mce {
namespace entity {
namespace parser {

struct entity_text_file_grammar;
struct entity_text_file_skipper;

/// Implements the front-end of the entity text file parser.
/**
 * Parses the text file into an AST.
 */
class entity_text_file_parser_frontend {
	/// Stores the grammar of the parsers.
	std::unique_ptr<entity_text_file_grammar> grammar;
	/// Stores the skip parser of the parser.
	std::unique_ptr<entity_text_file_skipper> skipper;

public:
	/// Constructs a parser front-end object.
	entity_text_file_parser_frontend();
	/// Destroys the parser front-end object.
	~entity_text_file_parser_frontend();

	/// \brief Parses the character sequence represented by first and last and uses the given filename for
	/// error messages.
	ast::ast_root parse(const std::string& filename, const char*& first, const char* last);
};

} // namespace parser
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_PARSER_FRONTEND_HPP_ */
