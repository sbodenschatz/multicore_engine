/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/parser/entity_text_file_parser.cpp
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
#pragma warning(disable : 4503)
#pragma warning(disable : 4244)
#pragma warning(disable : 4714)
#pragma warning(disable : 4127)
#pragma warning(disable : 4100)
#pragma warning(disable : 4348)
#endif
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <mce/entity/parser/entity_text_file_ast.hpp>
#include <mce/entity/parser/entity_text_file_ast_fusion.hpp>
#include <mce/entity/parser/entity_text_file_parser_frontend.hpp>
#include <mce/util/error_helper.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace mce {
namespace entity {
namespace parser {

struct entity_text_file_skipper : qi::grammar<const char*> {
	qi::rule<const char*> skip;
	entity_text_file_skipper() : entity_text_file_skipper::base_type(skip, "Skipper") {
		using qi::lit;
		using qi::char_;
		using qi::eol;
		using qi::eoi;
		skip = (spirit::ascii::space | (lit("//") >> *((char_ - eol) - eoi) >> (eol | eoi)));
	}
};

struct entity_text_file_grammar : qi::grammar<const char*, entity_text_file_skipper, ast::ast_root()> {
	template <typename Signature>
	using rule = qi::rule<const char*, entity_text_file_skipper, Signature>;

	rule<ast::ast_root()> start;
	rule<std::string()> identifier;
	rule<ast::root_element()> root_element;
	rule<ast::include_instruction()> include_instruction;
	rule<ast::entity_instance()> entity_instance;
	rule<ast::entity_definition()> entity_definition;
	rule<ast::component_definition()> component_definition;
	rule<ast::variable()> variable;
	rule<ast::variable_value()> variable_value;
	rule<ast::entity_reference()> entity_reference;
	rule<ast::marker_evaluation()> marker_evaluation;
	// rule<ast::marker_attribute()> marker_attribute;
	rule<ast::rotation_list()> rotation_list;
	rule<ast::rotation_element()> rotation_element;
	rule<ast::rotation_axis()> rotation_axis;
	rule<ast::int_list()> int_list;
	rule<ast::float_list()> float_list;
	rule<ast::string_list()> string_list;
	rule<std::string()> string_literal;
	rule<ast::entity_instance_param()> entity_instance_param;
	rule<ast::float_node> float_literal;

	entity_text_file_grammar() : entity_text_file_grammar::base_type(start) {
		using qi::_1;
		using spirit::_val;
		using phoenix::at_c;
		using qi::on_error;
		using qi::fail;
		using qi::debug;
		using qi::lit;
		using qi::char_;
		using qi::lexeme;
		using qi::no_case;
		using spirit::long_long;
		using spirit::float_;
		start %= *(root_element);
		identifier %= lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
		root_element =
				include_instruction[_val = _1] | entity_definition[_val = _1] | entity_instance[_val = _1];
		include_instruction %= lit("include") > string_literal > lit(";");
		entity_instance = identifier[at_c<0>(_val) = _1] >> -(identifier[at_c<1>(_val) = _1]) >>
						  entity_instance_param[at_c<2>(_val) = _1] >> lit(",") >>
						  entity_instance_param[at_c<3>(_val) = _1] >> lit(";");
		entity_definition = identifier[at_c<0>(_val) = _1] >> -(lit(":") > identifier[at_c<1>(_val) = _1]) >>
							lit("{") >> (*component_definition)[at_c<2>(_val) = _1] >> lit("}");
		component_definition = -(lit("replace")[at_c<0>(_val) = true]) >> identifier[at_c<1>(_val) = _1] >>
							   lit("{") >> (*variable)[at_c<2>(_val) = _1] >> lit("}");
		variable %= identifier >> lit("=") >> variable_value >> lit(";");
		variable_value = (long_long[_val = _1] >> !lit('.')) | float_literal[_val = _1] |
						 string_literal[_val = _1] | int_list[_val = _1] | float_list[_val = _1] |
						 string_list[_val = _1] | rotation_list[_val = _1] | marker_evaluation[_val = _1] |
						 entity_reference[_val = _1];
		entity_reference = lit("entity ") >> identifier[at_c<0>(_val) = _1];
		marker_evaluation = lit("marker ") >> string_literal[at_c<0>(_val) = _1];
		// marker_attribute;
		rotation_list = lit("(") >> ((rotation_element % ',')[_val = _1]) >> lit(")");
		rotation_element %= rotation_axis >> lit(":") >> float_;
		rotation_axis =
				no_case[lit("x")[_val = ast::rotation_axis::x] | lit("y")[_val = ast::rotation_axis::y] |
						lit("z")[_val = ast::rotation_axis::z]];
		int_list = (lit("(") >> lit(")")) | (lit("(") >> ((long_long % ',')[_val = _1]) >> lit(")"));
		float_list = lit("(") >> ((float_literal % ',')[_val = _1]) >> lit(")");
		string_list = lit("(") >> ((string_literal % ',')[_val = _1]) >> lit(")");
		string_literal %= lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
		entity_instance_param = int_list[_val = _1] | float_list[_val = _1] | rotation_list[_val = _1] |
								marker_evaluation[_val = _1] | entity_reference[_val = _1];
		float_literal = float_[at_c<0>(_val) = _1];

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(identifier);
		BOOST_SPIRIT_DEBUG_NODE(root_element);
		BOOST_SPIRIT_DEBUG_NODE(include_instruction);
		BOOST_SPIRIT_DEBUG_NODE(entity_instance);
		BOOST_SPIRIT_DEBUG_NODE(entity_definition);
		BOOST_SPIRIT_DEBUG_NODE(component_definition);
		BOOST_SPIRIT_DEBUG_NODE(variable);
		BOOST_SPIRIT_DEBUG_NODE(variable_value);
		BOOST_SPIRIT_DEBUG_NODE(entity_reference);
		BOOST_SPIRIT_DEBUG_NODE(marker_evaluation);
		// BOOST_SPIRIT_DEBUG_NODE(marker_attribute);
		BOOST_SPIRIT_DEBUG_NODE(rotation_list);
		BOOST_SPIRIT_DEBUG_NODE(rotation_element);
		BOOST_SPIRIT_DEBUG_NODE(rotation_axis);
		BOOST_SPIRIT_DEBUG_NODE(int_list);
		BOOST_SPIRIT_DEBUG_NODE(float_list);
		BOOST_SPIRIT_DEBUG_NODE(string_list);
		BOOST_SPIRIT_DEBUG_NODE(string_literal);
		BOOST_SPIRIT_DEBUG_NODE(entity_instance_param);
		BOOST_SPIRIT_DEBUG_NODE(float_literal);
	}
};

entity_text_file_parser_frontend::entity_text_file_parser_frontend()
		: grammar(std::make_unique<entity_text_file_grammar>()),
		  skipper(std::make_unique<entity_text_file_skipper>()) {}
entity_text_file_parser_frontend::~entity_text_file_parser_frontend() {}

ast::ast_root entity_text_file_parser_frontend::parse(const std::string& filename, const char*& first,
													  const char* last) {
	ast::ast_root ast_root;
	const char* buffer_start = first;
	try {
		bool result = qi::phrase_parse(first, last, *grammar, *skipper, ast_root);
		if(!result || !std::all_of(first, last, [](char c) {
			   return c == ' ' || c == '\t' || c == '\0' || c == '\n';
		   })) {
			util::throw_syntax_error(filename, buffer_start, first, "General syntax error");
		}
	} catch(boost::spirit::qi::expectation_failure<const char*>& ef) {
		util::throw_syntax_error(filename, buffer_start, ef.first, "Syntax error", ef.what_);
	}
	return ast_root;
}

} // namespace parser
} // namespace entity
} // namespace mce
