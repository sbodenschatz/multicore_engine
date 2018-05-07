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
#pragma warning(disable : 4127)
#endif
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <mce/asset_gen/pack_file_description_ast.hpp>
#include <mce/asset_gen/pack_file_description_ast_fusion.hpp>
#include <mce/asset_gen/pack_file_description_parser.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/error_helper.hpp>

namespace x3 = boost::spirit::x3;
namespace ascii = boost::spirit::x3::ascii;

namespace mce {
namespace asset_gen {
namespace parser {
namespace pack_file_description_parser_impl {
using x3::char_;
using x3::eoi;
using x3::eol;
using x3::eps;
using x3::int_;
using x3::lexeme;
using x3::lit;
using x3::no_case;
using x3::rule;

auto skipper = ascii::space | (lit("//") >> *((char_ - eol) - eoi));
rule<class pack_file_ast_root, ast::pack_file_ast_root> start;
rule<class pack_file_section, ast::pack_file_section> section;
rule<class pack_file_entry, ast::pack_file_entry> entry;
rule<class pack_file_string_literal, std::string> string_literal;
rule<class pack_file_identifier, std::string> identifier;
rule<class pack_file_zip_level, int> zip_level;
rule<class pack_file_lookup_spec, ast::lookup_type> lookup_spec;

struct lookup_type_ : x3::symbols<ast::lookup_type> {
	lookup_type_() {
		add("w", ast::lookup_type::w);
		add("d", ast::lookup_type::d);
	}
} lookup_type;

auto identifier_def = lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")];
auto string_literal_def = lexeme[lit('\"') > *((char_ - '\"')) > lit('\"')];
auto lookup_spec_def = no_case[lookup_type];
auto entry_def = string_literal > -(lookup_spec) > -(lit('-') > lit('>') > string_literal) > lit(';');
auto set_default_zip_level = [](auto& ctx) { _val(ctx) = -1; };
auto set_uncompressed_zip_level = [](auto& ctx) { _val(ctx) = -2; };
auto zip_level_def = ((lit("zip(") > int_ > lit(')')))	 // Explicit level
					 | (lit("zip")[set_default_zip_level]) // Default level
					 | (eps[set_uncompressed_zip_level]);  // Uncompressed
auto section_def = identifier > zip_level > lit('{') > *(entry) > lit('}');
auto start_def = *(section);

BOOST_SPIRIT_DEFINE(start);
BOOST_SPIRIT_DEFINE(section);
BOOST_SPIRIT_DEFINE(entry);
BOOST_SPIRIT_DEFINE(string_literal);
BOOST_SPIRIT_DEFINE(identifier);
BOOST_SPIRIT_DEFINE(lookup_spec);
BOOST_SPIRIT_DEFINE(zip_level);

} // namespace pack_file_description_parser_impl

ast::pack_file_ast_root pack_file_description_parser::parse(const std::string& filename, const char*& first,
															const char* last) {
	ast::pack_file_ast_root ast_root;
	const char* buffer_start = first;
	try {
		bool r = phrase_parse(first, last, pack_file_description_parser_impl::start,
							  pack_file_description_parser_impl::skipper, ast_root);
		if(!r || !std::all_of(first, last,
							  [](char c) { return c == ' ' || c == '\t' || c == '\0' || c == '\n'; })) {
			util::throw_syntax_error(filename, buffer_start, first, "General syntax error");
		}
	} catch(x3::expectation_failure<const char*>& ef) {
		util::throw_syntax_error(filename, buffer_start, ef.where(), "Syntax error", ef.which());
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
