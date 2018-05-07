/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/parser/entity_template_lang_parser_frontend.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEMPLATE_LANG_PARSER_FRONTEND_HPP_
#define ENTITY_PARSER_ENTITY_TEMPLATE_LANG_PARSER_FRONTEND_HPP_

/**
 * \file
 * Defines the frontend parser class for the entity template language.
 */

#include <mce/entity/parser/entity_template_lang_ast.hpp>
#include <memory>
#include <string>

namespace mce {
namespace entity {
namespace parser {

/// Implements the front-end of the entity template language parser.
/**
 * Parses the text file into an AST.
 */
class entity_template_lang_parser_frontend {
public:
	/// Constructs a parser front-end object.
	entity_template_lang_parser_frontend() = default;

	/// \brief Parses the character sequence represented by first and last and uses the given filename for
	/// error messages.
	ast::ast_root parse(const std::string& filename, const char*& first, const char* last);
};

} // namespace parser
} // namespace entity
} // namespace mce

#endif /* ENTITY_PARSER_ENTITY_TEMPLATE_LANG_PARSER_FRONTEND_HPP_ */
