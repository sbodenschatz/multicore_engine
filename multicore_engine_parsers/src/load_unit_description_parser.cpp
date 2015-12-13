/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/load_unit_description_parser.cpp
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
#include <asset_gen/load_unit_description_parser.hpp>
#include <asset_gen/load_unit_description_ast.hpp>
#include <asset_gen/load_unit_description_ast_fusion.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace mce {
namespace asset_gen {
namespace parser {

struct load_unit_description_skipper : qi::grammar<const char*> {
	qi::rule<const char*> skip;
	load_unit_description_skipper() : load_unit_description_skipper::base_type(skip, "Skipper") {
		using qi::lit;
		using qi::char_;
		using qi::eol;
		using qi::eoi;
		skip = (spirit::ascii::space | (lit("//") >> *((char_ - eol) - eoi) >> (eol | eoi)));
	}
};

struct load_unit_description_grammar
		: qi::grammar<const char*, load_unit_description_skipper, ast::load_unit_ast_root()> {
	template <typename Signature>
	using rule = qi::rule<const char*, load_unit_description_skipper, Signature>;

	rule<ast::load_unit_ast_root()> start;
	rule<ast::load_unit_section()> section;
	rule<ast::load_unit_entry()> entry;
	rule<std::string()> string_literal;

	load_unit_description_grammar() : load_unit_description_grammar::base_type(start) {
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

		string_literal %= lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
		entry %= string_literal >> -(lit('-') >> lit('>') >> string_literal) >> lit(';');
		section %= string_literal >> lit('{') >> *(entry) >> lit('}');
		start %= *(section);

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(section);
		BOOST_SPIRIT_DEBUG_NODE(entry);
	}
};

load_unit_description_parser::load_unit_description_parser()
		: grammar(std::make_unique<load_unit_description_grammar>()),
		  skipper(std::make_unique<load_unit_description_skipper>()) {}
load_unit_description_parser::~load_unit_description_parser() {}

bool load_unit_description_parser::parse(const char*& first, const char* last,
										 ast::load_unit_ast_root& ast_root) {
	bool result = qi::phrase_parse(first, last, *grammar, *skipper, ast_root);
	return result;
}

} // namespace parser
} // namespace asset_gen
} // namespace mce
