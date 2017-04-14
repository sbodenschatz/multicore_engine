/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/load_unit_description_parser.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

//#define BOOST_SPIRIT_DEBUG

#include <asset_gen/base_ast.hpp>
#include <boost/phoenix/core/actor.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4459)
#pragma warning(disable : 4503)
#pragma warning(disable : 4244)
#pragma warning(disable : 4714)
#pragma warning(disable : 4127)
#pragma warning(disable : 4100)
#pragma warning(disable : 4348)
#endif
#include <asset_gen/load_unit_description_ast.hpp>
#include <asset_gen/load_unit_description_ast_fusion.hpp>
#include <asset_gen/load_unit_description_parser.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <exceptions.hpp>
#include <util/error_helper.hpp>

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
	rule<std::string()> identifier;
	rule<ast::lookup_type> lookup_spec;

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

		identifier %= lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
		string_literal %= lexeme[lit('\"') > *((char_ - '\"')) > lit('\"')];
		lookup_spec = no_case[lit("w")[_val = ast::lookup_type::w] | lit("d")[_val = ast::lookup_type::d]];
		entry %= string_literal > -(lookup_spec) > -(lit('-') > lit('>') > string_literal) > lit(';');
		section %= identifier > lit('{') > *(entry) > lit('}');
		start %= *(section);

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(section);
		BOOST_SPIRIT_DEBUG_NODE(entry);
		BOOST_SPIRIT_DEBUG_NODE(identifier);
		BOOST_SPIRIT_DEBUG_NODE(string_literal);

		on_error<qi::rethrow>(entry, [](auto...) {});
		on_error<qi::rethrow>(section, [](auto...) {});
	}
};

load_unit_description_parser::load_unit_description_parser()
		: grammar(std::make_unique<load_unit_description_grammar>()),
		  skipper(std::make_unique<load_unit_description_skipper>()) {}
load_unit_description_parser::~load_unit_description_parser() {}

ast::load_unit_ast_root load_unit_description_parser::parse(const std::string& filename, const char*& first,
															const char* last) {
	ast::load_unit_ast_root ast_root;
	const char* buffer_start = first;
	try {
		bool r = qi::phrase_parse(first, last, *grammar, *skipper, ast_root);
		if(!r ||
		   !std::all_of(first, last,
						[](char c) { return c == ' ' || c == '\t' || c == '\0' || c == '\n'; })) {
			util::throw_syntax_error(filename, buffer_start, first, "General syntax error");
		}
	} catch(boost::spirit::qi::expectation_failure<const char*>& ef) {
		util::throw_syntax_error(filename, buffer_start, ef.first, "Syntax error", ef.what_);
	}
	return ast_root;
}

ast::load_unit_ast_root load_unit_description_parser::load_file(const std::string& filename) {
	std::ifstream stream(filename);
	std::vector<char> buffer;
	if(!stream.is_open()) {
		throw path_not_found_exception("Couldn't open file '" + filename + "'.");
	}

	std::copy(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(),
			  std::back_inserter(buffer));

	const char* start = buffer.data();
	const char* end = buffer.data() + buffer.size();
	return parse(filename, start, end);
}

} // namespace parser
} // namespace asset_gen
} // namespace mce
