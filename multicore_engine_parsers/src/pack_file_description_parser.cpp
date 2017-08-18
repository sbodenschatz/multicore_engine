/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/pack_file_description_parser.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

//#define BOOST_SPIRIT_DEBUG

#include <algorithm>
#include <boost/phoenix/core/actor.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <fstream>
#include <iterator>
#include <mce/asset_gen/base_ast.hpp>
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
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
#include <mce/asset_gen/pack_file_description_ast.hpp>
#include <mce/asset_gen/pack_file_description_ast_fusion.hpp>
#include <mce/asset_gen/pack_file_description_parser.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/error_helper.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace mce {
namespace asset_gen {
namespace parser {

struct pack_file_description_skipper : qi::grammar<const char*> {
	qi::rule<const char*> skip;
	pack_file_description_skipper() : pack_file_description_skipper::base_type(skip, "Skipper") {
		using qi::lit;
		using qi::char_;
		using qi::eol;
		using qi::eoi;
		skip = (spirit::ascii::space | (lit("//") >> *((char_ - eol) - eoi) >> (eol | eoi)));
	}
};

struct pack_file_description_grammar
		: qi::grammar<const char*, pack_file_description_skipper, ast::pack_file_ast_root()> {
	template <typename Signature>
	using rule = qi::rule<const char*, pack_file_description_skipper, Signature>;

	rule<ast::pack_file_ast_root()> start;
	rule<ast::pack_file_section()> section;
	rule<ast::pack_file_entry()> entry;
	rule<std::string()> string_literal;
	rule<std::string()> identifier;
	rule<int> zip_level;
	rule<int> integer_zip_level;
	rule<ast::lookup_type> lookup_spec;

	pack_file_description_grammar() : pack_file_description_grammar::base_type(start) {
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
		using spirit::int_;
		using qi::eps;

		identifier %= lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
		string_literal %= lexeme[lit('\"') > *((char_ - '\"')) > lit('\"')];
		lookup_spec = no_case[lit("w")[_val = ast::lookup_type::w] | lit("d")[_val = ast::lookup_type::d]];
		entry %= string_literal > -(lookup_spec) > -(lit('-') > lit('>') > string_literal) > lit(';');
		integer_zip_level %= int_;
		zip_level = ((lit("zip(") > integer_zip_level > lit(')'))[_val = _1]) // Explicit level
					| (lit("zip")[_val = -1])								  // Default level
					| (eps[_val = -2]);										  // Uncompressed
		section %= identifier > zip_level > lit('{') > *(entry) > lit('}');
		start %= *(section);

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(section);
		BOOST_SPIRIT_DEBUG_NODE(entry);
		BOOST_SPIRIT_DEBUG_NODE(identifier);
		BOOST_SPIRIT_DEBUG_NODE(string_literal);
		BOOST_SPIRIT_DEBUG_NODE(zip_level);
		BOOST_SPIRIT_DEBUG_NODE(integer_zip_level);

		on_error<qi::rethrow>(entry, [](auto...) {});
		on_error<qi::rethrow>(section, [](auto...) {});
		on_error<qi::rethrow>(zip_level, [](auto...) {});
	}
};

pack_file_description_parser::pack_file_description_parser()
		: grammar(std::make_unique<pack_file_description_grammar>()),
		  skipper(std::make_unique<pack_file_description_skipper>()) {}
pack_file_description_parser::~pack_file_description_parser() {}

ast::pack_file_ast_root pack_file_description_parser::parse(const std::string& filename, const char*& first,
															const char* last) {
	ast::pack_file_ast_root ast_root;
	const char* buffer_start = first;
	try {
		bool r = qi::phrase_parse(first, last, *grammar, *skipper, ast_root);
		if(!r || !std::all_of(first, last, [](char c) {
			   return c == ' ' || c == '\t' || c == '\0' || c == '\n';
		   })) {
			util::throw_syntax_error(filename, buffer_start, first, "General syntax error");
		}
	} catch(boost::spirit::qi::expectation_failure<const char*>& ef) {
		util::throw_syntax_error(filename, buffer_start, ef.first, "Syntax error", ef.what_);
	}
	return ast_root;
}
ast::pack_file_ast_root pack_file_description_parser::load_file(const std::string& filename) {
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
