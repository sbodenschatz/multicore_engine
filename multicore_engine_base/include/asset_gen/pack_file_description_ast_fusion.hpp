/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_description_ast_fusion.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_DESCRIPTION_AST_FUSION_HPP_
#define ASSET_GEN_PACK_FILE_DESCRIPTION_AST_FUSION_HPP_

#include "pack_file_description_ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(mce::asset_gen::ast::pack_file_entry,		 //
						  (std::string, external_path)				 //
						  (mce::asset_gen::ast::lookup_type, lookup) //
						  (std::string, internal_path))				 //

BOOST_FUSION_ADAPT_STRUCT(mce::asset_gen::ast::pack_file_section,						//
						  (std::string, name)											//
						  (int, zip_level)												//
						  (std::vector<mce::asset_gen::ast::pack_file_entry>, entries)) //

#endif /* ASSET_GEN_PACK_FILE_DESCRIPTION_AST_FUSION_HPP_ */
