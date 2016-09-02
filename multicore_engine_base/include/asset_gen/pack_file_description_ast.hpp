/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_ast.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_

#include "base_ast.hpp"
#include <ostream>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {
namespace ast {

struct pack_file_entry {
	// cppcheck-suppress passedByValue
	pack_file_entry(std::string external_path, lookup_type lookup, std::string internal_path)
			: external_path(std::move(external_path)), lookup(lookup),
			  internal_path(std::move(internal_path)) {}
	// cppcheck-suppress passedByValue
	pack_file_entry(std::string external_path, std::string internal_path)
			: external_path(std::move(external_path)), internal_path(std::move(internal_path)) {}
	pack_file_entry() {}
	std::string external_path;
	lookup_type lookup = lookup_type::w;
	std::string internal_path;
};

struct pack_file_section {
	std::string name;
	int zip_level;
	std::vector<pack_file_entry> entries;
};

typedef std::vector<pack_file_section> pack_file_ast_root;

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_ */
