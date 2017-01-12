/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/base_ast.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_BASE_AST_HPP_
#define ASSET_GEN_BASE_AST_HPP_

#include <ostream>

namespace mce {
namespace asset_gen {
namespace ast {

/// @brief Represents a lookup type modifier in description files and is used to specify which base is used
/// for relative paths.
enum class lookup_type {
	w, ///< Resolve paths relative to the tools working directory.
	d  ///< Resolve paths relative to the directory of the description file.
};

/// Debug output operator for #mce::asset_gen::ast::lookup_type
inline std::ostream& operator<<(std::ostream& s, const lookup_type& r) {
	if(r == lookup_type::w) {
		s << "w";
	} else if(r == lookup_type::d) {
		s << "d";
	} else {
		s << "<invalid lookup_type>";
	}
	return s;
}

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_BASE_AST_HPP_ */
