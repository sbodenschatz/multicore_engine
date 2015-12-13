/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_description_ast_fusion.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_FUSION_HPP_
#define ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_FUSION_HPP_

#include "load_unit_description_ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(mce::asset_gen::ast::load_unit_entry, //
						  (std::string, external_path)			//
						  (std::string, internal_path))			//

BOOST_FUSION_ADAPT_STRUCT(mce::asset_gen::ast::load_unit_section,						//
						  (std::string, name)											//
						  (std::vector<mce::asset_gen::ast::load_unit_entry>, entries)) //

#endif /* ASSET_GEN_LOAD_UNIT_DESCRIPTION_AST_FUSION_HPP_ */
