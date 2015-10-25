/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/parser/entity_text_file_parser.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

//#define BOOST_SPIRIT_DEBUG
#include <istream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#pragma warning(disable : 4459)
#pragma warning(disable : 4503)
#pragma warning(disable : 4244)
#pragma warning(disable : 4714)
#pragma warning(disable : 4127)
#pragma warning(disable : 4100)
#endif
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <entity/parser/entity_text_file_parser.hpp>
#include <entity/parser/entity_text_file_ast.hpp>
#include <entity/parser/entity_text_file_ast_fusion.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace mce {
namespace entity {
namespace parser {

typedef boost::spirit::ascii::space_type entity_text_file_skipper;

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
		identifier %= lexeme[char_("a-fA-F_") >> *char_("0-9a-fA-F_")];
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
		variable_value = long_long[_val = _1] | float_[_val = _1] | string_literal[_val = _1] |
						 int_list[_val = _1] | float_list[_val = _1] | string_list[_val = _1] |
						 rotation_list[_val = _1] | marker_evaluation[_val = _1] |
						 entity_reference[_val = _1];
		entity_reference = lit("entity") >> identifier[at_c<0>(_val) = _1];
		marker_evaluation = lit("marker") >> string_literal[at_c<0>(_val) = _1];
		// marker_attribute;
		rotation_list = (lit("(") >> lit(")")) | ((rotation_element % ',')[_val = _1]);
		rotation_element %= rotation_axis >> lit(":") >> float_;
		rotation_axis =
				no_case[lit("x")[_val = ast::rotation_axis::x] | lit("y")[_val = ast::rotation_axis::y] |
						lit("z")[_val = ast::rotation_axis::z]];
		int_list = (lit("(") >> lit(")")) | ((long_long % ',')[_val = _1]);
		float_list = (lit("(") >> lit(")")) | ((float_ % ',')[_val = _1]);
		string_list = (lit("(") >> lit(")")) | ((string_literal % ',')[_val = _1]);
		string_literal %= lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
		entity_instance_param = int_list[_val = _1] | float_list[_val = _1] | rotation_list[_val = _1] |
								marker_evaluation[_val = _1] | entity_reference[_val = _1];
	}
};

entity_text_file_parser_frontend::entity_text_file_parser_frontend()
		: grammar(std::make_unique<entity_text_file_grammar>()) {}
entity_text_file_parser_frontend::~entity_text_file_parser_frontend() {}

void entity_text_file_parser_frontend::parse(const char* first, const char* last, ast::ast_root& ast_root) {
	(void)first;
	(void)last;
	(void)ast_root;
}

} // namespace parser
} // namespace entity
} // namespace mce
