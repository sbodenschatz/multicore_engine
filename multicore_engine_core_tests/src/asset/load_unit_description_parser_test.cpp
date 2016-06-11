/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/asset/load_unit_description_parser.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset_gen/load_unit_description_ast.hpp>
#include <asset_gen/load_unit_description_ast_compare.hpp>
#include <asset_gen/load_unit_description_parser.hpp>
#include <boost/test/unit_test.hpp>

namespace mce {
namespace asset_gen {
namespace parser {

BOOST_AUTO_TEST_SUITE(assets)
BOOST_AUTO_TEST_SUITE(load_unit_description_parser_test)

BOOST_AUTO_TEST_CASE(empty_file_valid) {
	load_unit_description_parser parser;
	std::string testdata = "";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}
BOOST_AUTO_TEST_CASE(only_comment_valid) {
	load_unit_description_parser parser;
	std::string testdata = "//Comment\n";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}

BOOST_AUTO_TEST_CASE(single_empty_section) {
	load_unit_description_parser parser;
	std::string testdata = "test{}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_comment_in_section) {
	load_unit_description_parser parser;
	std::string testdata = "test{//Comment\n}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(single_section_single_entry) {
	load_unit_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {{"test1", ast::lookup_type::w, "test2"}}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(single_section_multi_entry) {
	load_unit_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test",
								{{"test1", ast::lookup_type::w, "test2"},
								 {"test3", ast::lookup_type::w, "test4"},
								 {"test5", ast::lookup_type::w, "test6"}}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(multi_section_single_entry) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";}testB{\"test1\"->\"test2\";}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA", {{"test1", ast::lookup_type::w, "test2"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB", {{"test1", ast::lookup_type::w, "test2"}}});
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(multi_section_multi_entry) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"d->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"d->\"test6\";}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::d, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::d, "test6"}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(no_internal_path) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\";\"test3\"D;\"test5\";}"
						   "testB{\"test1\";\"test3\";\"test5\"d;}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, {}},
													   {"test3", ast::lookup_type::d, {}},
													   {"test5", ast::lookup_type::w, {}}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, {}},
													   {"test3", ast::lookup_type::w, {}},
													   {"test5", ast::lookup_type::d, {}}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(whitespace_skipping) {
	load_unit_description_parser parser;
	std::string testdata =
			"testA\n{\"test1\"\n->\n\"test2\";\"test3\" -> \"test4\"\n;\"test5\"\t-> \"test6\";}"
			"testB{\n\"test1\" ->\n\"test2\";\"test3\"\t->\t\"test4\"   ;\n    \"test5\"   ->    "
			"\"test6\"//test\n;}";
	ast::load_unit_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace parser
} // namespace asset_gen
} // namespace mce
