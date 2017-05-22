/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/parser/entity_text_file_ast_fusion.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_PARSER_ENTITY_TEXT_FILE_AST_FUSION_HPP_
#define ENTITY_PARSER_ENTITY_TEXT_FILE_AST_FUSION_HPP_

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/mpl/sequence_tag.hpp>
#include <mce/entity/parser/entity_text_file_ast.hpp>
#include <string>
#include <vector>

/**
 * \file
 * Adaptation of entity text file AST nodes for boost.fusion.
 */

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::float_node, (float, value))

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::rotation_element,	 //
						  (mce::entity::ast::rotation_axis, axis) //
						  (float, angle))						  //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::marker_evaluation, //
						  //(mce::entity::ast::marker_attribute, attribute) //
						  (std::string, marker_name)) //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::entity_reference, //
						  (std::string, referred_name))		  //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::variable, (std::string, name) //
						  (mce::entity::ast::variable_value, value))	  //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::component_definition, (bool, replace) //
						  (std::string, name)									  //
						  (std::vector<mce::entity::ast::variable>, variables))   //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::entity_definition,								 //
						  (std::string, name)												 //
						  (std::string, super_name)											 //
						  (std::vector<mce::entity::ast::component_definition>, components)) //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::include_instruction, //
						  (std::string, filename))				 //

BOOST_FUSION_ADAPT_STRUCT(mce::entity::ast::entity_instance,								//
						  (std::string, type_name)											//
						  (std::string, instance_name)										//
						  (mce::entity::ast::entity_instance_param, position_parameter)		//
						  (mce::entity::ast::entity_instance_param, orientation_parameter)) //

#endif /* ENTITY_PARSER_ENTITY_TEXT_FILE_AST_FUSION_HPP_ */
