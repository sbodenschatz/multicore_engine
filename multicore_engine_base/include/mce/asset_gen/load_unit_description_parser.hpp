/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset_gen/load_unit_description_parser.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_

/**
 * \file
 * Defines the parser class for load unit descriptions.
 */

#include <mce/asset_gen/load_unit_description_ast.hpp>
#include <memory>
#include <string>

namespace mce {
namespace asset_gen {
namespace parser {

/// Implements the parser front end for load unit description files.
class load_unit_description_parser {
public:
	/// Constructs the load unit description parser.
	load_unit_description_parser() = default;
	/// Parses the given character sequence using the given filename for error tracking.
	ast::load_unit_ast_root parse(const std::string& filename, const char*& first, const char* last);
	/// Loads and parses the named file.
	ast::load_unit_ast_root load_file(const std::string& filename);
};

} // namespace parser
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_ */
