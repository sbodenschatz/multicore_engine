/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_description_ast_compare.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_COMPARE_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_COMPARE_HPP_

#include "load_unit_description_ast.hpp"

namespace mce {
namespace asset_gen {
namespace ast {

inline bool operator==(const load_unit_entry& o1, const load_unit_entry& o2) {
	return o1.external_path == o2.external_path && o1.lookup == o2.lookup &&
		   o1.internal_path == o2.internal_path;
}
inline bool operator!=(const load_unit_entry& o1, const load_unit_entry& o2) {
	return !(o1 == o2);
}
inline bool operator==(const load_unit_section& o1, const load_unit_section& o2) {
	return o1.name == o2.name && o1.entries == o2.entries;
}
inline bool operator!=(const load_unit_section& o1, const load_unit_section& o2) {
	return !(o1 == o2);
}

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_COMPARE_HPP_ */
