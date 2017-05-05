/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_ast.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_

/**
 * \file
 * Specifies the AST node types for pack file description files.
 */

#include <algorithm>
#include <asset_gen/base_ast.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {
namespace ast {

/// AST node for a parsed entry of a pack file description.
struct pack_file_entry {
	/// Construct pack file entry node from values with lookup type.
	// cppcheck-suppress passedByValue
	pack_file_entry(std::string external_path, lookup_type lookup, std::string internal_path)
			: external_path(std::move(external_path)), lookup(lookup),
			  internal_path(std::move(internal_path)) {}
	/// Construct pack file entry node from values without lookup type.
	// cppcheck-suppress passedByValue
	pack_file_entry(std::string external_path, std::string internal_path)
			: external_path(std::move(external_path)), internal_path(std::move(internal_path)) {}
	/// Construct empty pack file entry node.
	pack_file_entry() {}
	std::string external_path;			 ///< The external path of the file to add into the pack file.
	lookup_type lookup = lookup_type::w; ///< Specifies how the external path is resolved when relative.
	std::string internal_path;			 ///< The virtual name the file will have in the pack file.
};

/// AST node for a parsed section of a pack file description.
struct pack_file_section {
	std::string name; ///< The name of the pack file section.

	/// The compression level to use for this section.
	/**
	 * The value has the following semantics:
	 * Explicit levels range from 0 to 9 represent low to high compression levels accordingly and are
	 * generated from zip(n) modifiers with n in the given range.
	 * The default compression level of zlib is represented by the value of -1 which is parsed from a zip
	 * modifier without a number.
	 * A value of -2 disables compression for the section and is parsed if there is no zip modifier on the
	 * section.
	 */
	int zip_level;
	std::vector<pack_file_entry> entries; ///< The nodes for the entries in section.
};

/// AST root node for a parsed pack file description.
typedef std::vector<pack_file_section> pack_file_ast_root;

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_ */
