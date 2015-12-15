/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_description_parser.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_

#include "load_unit_description_ast.hpp"
#include <memory>

namespace mce {
namespace asset_gen {
namespace parser {

struct load_unit_description_grammar;
struct load_unit_description_skipper;

class load_unit_description_parser {
	std::unique_ptr<load_unit_description_grammar> grammar;
	std::unique_ptr<load_unit_description_skipper> skipper;

public:
	load_unit_description_parser();
	~load_unit_description_parser();
	bool parse(const char*& first, const char* last, ast::load_unit_ast_root& ast_root);
	ast::load_unit_ast_root load_file(const std::string& filename);
};

} // namespace parser
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_PARSER_HPP_ */
