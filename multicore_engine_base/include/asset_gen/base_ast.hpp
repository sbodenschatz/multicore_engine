/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/base_ast.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_BASE_AST_HPP_
#define ASSET_GEN_BASE_AST_HPP_

#include <ostream>

namespace mce {
namespace asset_gen {
namespace ast {

enum class lookup_type { w, d };
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
