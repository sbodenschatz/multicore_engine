/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_ast.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_

#include <string>
#include <vector>
#include <ostream>

namespace mce {
namespace asset_gen {
namespace ast {

struct pack_file_entry {
	std::string external_path;
	std::string internal_path;
};

struct pack_file_section {
	std::string name;
	std::vector<pack_file_entry> entries;
};

typedef std::vector<pack_file_section> pack_file_ast_root;

} // namespace ast
} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_AST_HPP_ */
