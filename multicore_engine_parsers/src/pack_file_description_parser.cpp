/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/pack_file_description_parser.cpp
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
#include <asset_gen/pack_file_description_parser.hpp>
#include <asset_gen/pack_file_description_ast.hpp>
#include <asset_gen/pack_file_description_ast_fusion.hpp>

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

		identifier %= lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
		string_literal %= lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
		entry %= string_literal >> -(lit('-') >> lit('>') >> string_literal) >> lit(';');
		section %= identifier >> lit('{') >> *(entry) >> lit('}');
		start %= *(section);

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(section);
		BOOST_SPIRIT_DEBUG_NODE(entry);
	}
};

pack_file_description_parser::pack_file_description_parser()
		: grammar(std::make_unique<pack_file_description_grammar>()),
		  skipper(std::make_unique<pack_file_description_skipper>()) {}
pack_file_description_parser::~pack_file_description_parser() {}

bool pack_file_description_parser::parse(const char*& first, const char* last,
										 ast::pack_file_ast_root& ast_root) {
	bool result = qi::phrase_parse(first, last, *grammar, *skipper, ast_root);
	return result;
}

} // namespace parser
} // namespace asset_gen
} // namespace mce
