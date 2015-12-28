/*
 * Multi-Core Engine project
 * File /multicore_engine_parsers/src/pack_file_description_parser.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

//#define BOOST_SPIRIT_DEBUG

#include <fstream>
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
	rule<int> zip_level;

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
		string_literal %= lexeme[lit('\"') >> *((char_ - '\"')) >> lit('\"')];
		entry %= string_literal >> -(lit('-') >> lit('>') >> string_literal) >> lit(';');
		zip_level = ((lit("zip(") >> int_ >> lit(')'))[_val = _1]) // Explicit level
					| (lit("zip")[_val = -1])					   // Default level
					| (eps[_val = -2]);							   // Uncompressed
		section %= identifier >> zip_level >> lit('{') >> *(entry) >> lit('}');
		start %= *(section);

		BOOST_SPIRIT_DEBUG_NODE(start);
		BOOST_SPIRIT_DEBUG_NODE(section);
		BOOST_SPIRIT_DEBUG_NODE(entry);
		BOOST_SPIRIT_DEBUG_NODE(identifier);
		BOOST_SPIRIT_DEBUG_NODE(string_literal);
		BOOST_SPIRIT_DEBUG_NODE(zip_level);
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
ast::pack_file_ast_root pack_file_description_parser::load_file(const std::string& filename) {
	ast::pack_file_ast_root ast_root;
	std::ifstream stream(filename);
	std::vector<char> buffer;
	if(!stream.is_open()) {
		throw std::runtime_error("Couldn't open file '" + filename + "'.");
	}

	stream.seekg(0, std::ios::end);
	auto size_tmp = stream.tellg();
	size_t size = size_tmp;
	decltype(size_tmp) size_check = size;
	if(size_check != size_tmp) throw std::runtime_error("File too big to fit in address space.");
	stream.seekg(0, std::ios::beg);

	buffer.resize(size);
	stream.read(buffer.data(), size);

	const char* start = buffer.data();
	const char* end = buffer.data() + size;
	bool r = parse(start, end, ast_root);
	if(!r || start != end) {
		std::runtime_error("Parse error in file '" + filename + "'.");
	}
	return ast_root;
}

} // namespace parser
} // namespace asset_gen
} // namespace mce
