/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_description_ast.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_

#include <string>
#include <vector>
#include <ostream>
#include "base_ast.hpp"

namespace mce {
namespace asset_gen {
namespace ast {

struct load_unit_entry {
	load_unit_entry(const std::string& external_path, lookup_type lookup, const std::string& internal_path) :external_path(external_path), lookup(lookup), internal_path(internal_path) {}
	load_unit_entry(const std::string& external_path, const std::string& internal_path) :external_path(external_path), internal_path(internal_path) {}
	load_unit_entry(){}
	std::string external_path;
	lookup_type lookup = lookup_type::w;
	std::string internal_path;
};

struct load_unit_section {
	std::string name;
	std::vector<load_unit_entry> entries;
};

typedef std::vector<load_unit_section> load_unit_ast_root;

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_ */
