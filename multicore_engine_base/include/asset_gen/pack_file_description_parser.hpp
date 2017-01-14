/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_parser.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_PARSER_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_PARSER_HPP_

#include "pack_file_description_ast.hpp"
#include <memory>

namespace mce {
namespace asset_gen {
namespace parser {

struct pack_file_description_grammar;
struct pack_file_description_skipper;

/// Implements the parser front end for pack file description files.
class pack_file_description_parser {
	std::unique_ptr<pack_file_description_grammar> grammar;
	std::unique_ptr<pack_file_description_skipper> skipper;

public:
	/// Constructs the pack file description parser.
	pack_file_description_parser();
	/// Explicit destructor required because the grammar and skipper are only complete types in the cpp file.
	~pack_file_description_parser();
	/// Parses the given character sequence using the given filename for error tracking.
	ast::pack_file_ast_root parse(const std::string& filename, const char*& first, const char* last);
	/// Loads and parses the named file.
	ast::pack_file_ast_root load_file(const std::string& filename);
};

} // namespace parser
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_PARSER_HPP_ */
