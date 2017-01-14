/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_ast_compare.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_AST_COMPARE_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_AST_COMPARE_HPP_

#include "pack_file_description_ast.hpp"

namespace mce {
namespace asset_gen {
namespace ast {

/// Enables comparison of pack file entries for equal.
inline bool operator==(const pack_file_entry& o1, const pack_file_entry& o2) {
	return o1.external_path == o2.external_path && o1.lookup == o2.lookup &&
		   o1.internal_path == o2.internal_path;
}
/// Enables comparison of pack file entries for not equal.
inline bool operator!=(const pack_file_entry& o1, const pack_file_entry& o2) {
	return !(o1 == o2);
}
/// Enables comparison of pack file sections for equal.
inline bool operator==(const pack_file_section& o1, const pack_file_section& o2) {
	return o1.name == o2.name && o1.entries == o2.entries;
}
/// Enables comparison of pack file sections for not equal.
inline bool operator!=(const pack_file_section& o1, const pack_file_section& o2) {
	return !(o1 == o2);
}

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_AST_COMPARE_HPP_ */
