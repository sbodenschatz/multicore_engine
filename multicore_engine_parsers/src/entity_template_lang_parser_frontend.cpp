/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/entity/parser/entity_template_lang_parser.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

//#define BOOST_SPIRIT_DEBUG

#include <algorithm>
#include <boost/phoenix/core/actor.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <memory>
#include <string>

#ifdef _MSC_VER
#pragma warning(disable : 4459)
#pragma warning(disable : 4127)
#endif
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <mce/entity/parser/entity_template_lang_ast.hpp>
#include <mce/entity/parser/entity_template_lang_ast_fusion.hpp>
#include <mce/entity/parser/entity_template_lang_parser_frontend.hpp>
#include <mce/util/error_helper.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

namespace mce {
namespace entity {
namespace parser {
namespace entity_template_lang_parser_impl {
using x3::char_;
using x3::eoi;
using x3::eol;
using x3::eps;
using x3::float_;
using x3::int_;
using x3::lexeme;
using x3::lit;
using x3::long_long;
using x3::no_case;
using x3::rule;

auto skipper = (ascii::space | (lit("//") >> *((char_ - eol) - eoi) >> (eol | eoi)));

rule<class etl_start, ast::ast_root> start;
rule<class etl_identifier, std::string> identifier;
rule<class etl_root_element, ast::root_element> root_element;
rule<class etl_include_instruction, ast::include_instruction> include_instruction;
rule<class etl_entity_instance, ast::entity_instance> entity_instance;
rule<class etl_entity_definition, ast::entity_definition> entity_definition;
rule<class etl_component_definition, ast::component_definition> component_definition;
rule<class etl_variable, ast::variable> variable;
rule<class etl_variable_value, ast::variable_value> variable_value;
rule<class etl_entity_reference, ast::entity_reference> entity_reference;
rule<class etl_marker_evaluation, ast::marker_evaluation> marker_evaluation;
// rule<ast::marker_attribute> marker_attribute;
rule<class etl_rotation_list, ast::rotation_list> rotation_list;
rule<class etl_rotation_element, ast::rotation_element> rotation_element;
rule<class etl_rotation_axis, ast::rotation_axis> rotation_axis;
rule<class etl_int_list, ast::int_list> int_list;
rule<class etl_float_list, ast::float_list> float_list;
rule<class etl_string_list, ast::string_list> string_list;
rule<class etl_string_literal, std::string> string_literal;
rule<class etl_entity_instance_param, ast::entity_instance_param> entity_instance_param;
rule<class etl_float_literal, ast::float_node> float_literal;

auto start_def = *(root_element);
auto identifier_def = lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
auto root_element_def = include_instruction | entity_definition | entity_instance;
auto include_instruction_def = lit("include") > string_literal > lit(";");

auto set_type_name = [](auto ctx) { _val(ctx).type_name = _attr(ctx); };
auto set_instance_name = [](auto ctx) { _val(ctx).instance_name = _attr(ctx); };
auto set_position_param = [](auto ctx) { _val(ctx).position_parameter = _attr(ctx); };
auto set_orientation_param = [](auto ctx) { _val(ctx).orientation_parameter = _attr(ctx); };

auto entity_instance_def = identifier[set_type_name] >>
						   -(identifier[set_instance_name]) >> entity_instance_param[set_position_param] >>
						   lit(",") >> entity_instance_param[set_orientation_param] >> lit(";");

auto set_name = [](auto ctx) { _val(ctx).name = _attr(ctx); };
auto set_super_name = [](auto ctx) { _val(ctx).super_name = _attr(ctx); };
auto set_components = [](auto ctx) { _val(ctx).components = _attr(ctx); };

auto entity_definition_def = identifier[set_name] >> -(lit(":") > identifier[set_super_name]) >> lit("{") >>
							 (*component_definition)[set_components] >> lit("}");

auto enable_replace = [](auto ctx) { _val(ctx).replace = true; };
auto set_variables = [](auto ctx) { _val(ctx).variables = _attr(ctx); };

auto component_definition_def = -(lit("replace")[enable_replace]) >> identifier[set_name] >> lit("{") >>
								(*variable)[set_variables] >> lit("}");
auto variable_def = identifier >> lit("=") >> variable_value >> lit(";");
auto variable_value_def = (long_long >> !lit('.')) | float_literal | string_literal | int_list | float_list |
						  string_list | rotation_list | marker_evaluation | entity_reference;
auto entity_reference_def = lit("entity ") >> identifier;
auto marker_evaluation_def = lit("marker ") >> string_literal;
// marker_attribute;
auto rotation_list_def = lit("(") >> ((rotation_element % ',')) >> lit(")");
auto rotation_element_def = rotation_axis >> lit(":") >> float_;
struct rotation_axis_spec_ : x3::symbols<ast::rotation_axis> {
	rotation_axis_spec_() {
		add("x", ast::rotation_axis::x);
		add("y", ast::rotation_axis::y);
		add("z", ast::rotation_axis::z);
	}
} rotation_axis_spec;

auto rotation_axis_def = no_case[rotation_axis_spec];
auto int_list_def = (lit("(") >> lit(")")) | (lit("(") >> ((long_long % ',')) >> lit(")"));
auto float_list_def = lit("(") >> ((float_literal % ',')) >> lit(")");
auto string_list_def = lit("(") >> ((string_literal % ',')) >> lit(")");
auto string_literal_def = lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
auto entity_instance_param_def = int_list | float_list | rotation_list | marker_evaluation | entity_reference;
auto float_literal_def = float_;

BOOST_SPIRIT_DEFINE(start);
BOOST_SPIRIT_DEFINE(identifier);
BOOST_SPIRIT_DEFINE(root_element);
BOOST_SPIRIT_DEFINE(include_instruction);
BOOST_SPIRIT_DEFINE(entity_instance);
BOOST_SPIRIT_DEFINE(entity_definition);
BOOST_SPIRIT_DEFINE(component_definition);
BOOST_SPIRIT_DEFINE(variable);
BOOST_SPIRIT_DEFINE(variable_value);
BOOST_SPIRIT_DEFINE(entity_reference);
BOOST_SPIRIT_DEFINE(marker_evaluation);
// BOOST_SPIRIT_DEFINE(marker_attribute);
BOOST_SPIRIT_DEFINE(rotation_list);
BOOST_SPIRIT_DEFINE(rotation_element);
BOOST_SPIRIT_DEFINE(rotation_axis);
BOOST_SPIRIT_DEFINE(int_list);
BOOST_SPIRIT_DEFINE(float_list);
BOOST_SPIRIT_DEFINE(string_list);
BOOST_SPIRIT_DEFINE(string_literal);
BOOST_SPIRIT_DEFINE(entity_instance_param);
BOOST_SPIRIT_DEFINE(float_literal);

} // namespace entity_template_lang_parser_impl

ast::ast_root entity_template_lang_parser_frontend::parse(const std::string& filename, const char*& first,
														  const char* last) {
	ast::ast_root ast_root;
	const char* buffer_start = first;
	try {
		bool result = phrase_parse(first, last, entity_template_lang_parser_impl::start,
								   entity_template_lang_parser_impl::skipper, ast_root);
		if(!result || !std::all_of(first, last,
								   [](char c) { return c == ' ' || c == '\t' || c == '\0' || c == '\n'; })) {
			util::throw_syntax_error(filename, buffer_start, first, "General syntax error");
		}
	} catch(x3::expectation_failure<const char*>& ef) {
		util::throw_syntax_error(filename, buffer_start, ef.where(), "Syntax error", ef.which());
	}
	return ast_root;
}

} // namespace parser
} // namespace entity
} // namespace mce
