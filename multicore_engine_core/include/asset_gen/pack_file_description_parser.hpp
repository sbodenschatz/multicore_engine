/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_parser.hpp
 * Copyright 2015 by Stefan Bodenschatz
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

class pack_file_description_parser {
	std::unique_ptr<pack_file_description_grammar> grammar;
	std::unique_ptr<pack_file_description_skipper> skipper;

public:
	pack_file_description_parser();
	~pack_file_description_parser();
	bool parse(const char*& first, const char* last, ast::pack_file_ast_root& ast_root);
	void load_file(const std::string& filename);
};

} // namespace parser
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_PARSER_HPP_ */
