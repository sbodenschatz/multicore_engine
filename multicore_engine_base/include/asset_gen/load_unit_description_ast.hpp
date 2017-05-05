/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_description_ast.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_

/**
 * \file
 * Specifies the AST node types for load unit description files.
 */

#include <algorithm>
#include <asset_gen/base_ast.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {
namespace ast {

/// AST node for a parsed entry in a load unit description.
struct load_unit_entry {
	/// Construct load unit entry node from values with lookup type.
	// cppcheck-suppress passedByValue
	load_unit_entry(std::string external_path, lookup_type lookup, std::string internal_path)
			: external_path(std::move(external_path)), lookup(lookup),
			  internal_path(std::move(internal_path)) {}
	/// Construct load unit entry node from values without lookup type.
	// cppcheck-suppress passedByValue
	load_unit_entry(std::string external_path, std::string internal_path)
			: external_path(std::move(external_path)), internal_path(std::move(internal_path)) {}
	/// Construct empty load unit entry node.
	load_unit_entry() {}
	std::string external_path;			 ///< The external path of the file to add into the load unit.
	lookup_type lookup = lookup_type::w; ///< Specifies how the external path is resolved when relative.
	std::string internal_path;			 ///< The virtual name the asset will have in the load unit.
};

/// AST node for a parsed section in a load unit description.
struct load_unit_section {
	std::string name;					  ///< The name of the section.
	std::vector<load_unit_entry> entries; ///< The entries contained in the section description.
};

/// AST root node for a parsed load unit description.
typedef std::vector<load_unit_section> load_unit_ast_root;

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_HPP_ */
