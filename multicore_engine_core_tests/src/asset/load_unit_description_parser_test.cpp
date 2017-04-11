/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/asset/load_unit_description_parser.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset_gen/load_unit_description_ast.hpp>
#include <asset_gen/load_unit_description_ast_compare.hpp>
#include <asset_gen/load_unit_description_parser.hpp>
#include <exceptions.hpp>
#include <gtest.hpp>

namespace mce {
namespace asset_gen {
namespace parser {

TEST(assets_load_unit_description_parser_test, empty_file_valid) {
	load_unit_description_parser parser;
	std::string testdata = "";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ASSERT_TRUE(root.size() == 0);
}
TEST(assets_load_unit_description_parser_test, only_comment_valid) {
	load_unit_description_parser parser;
	std::string testdata = "//Comment\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ASSERT_TRUE(root.size() == 0);
}

TEST(assets_load_unit_description_parser_test, single_empty_section) {
	load_unit_description_parser parser;
	std::string testdata = "test{}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {}};
	root_expected.emplace_back(sec1);
	ASSERT_TRUE(root == root_expected);
}

TEST(assets_load_unit_description_parser_test, single_comment_in_section) {
	load_unit_description_parser parser;
	std::string testdata = "test{//Comment\n}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {}};
	root_expected.emplace_back(sec1);
	ASSERT_TRUE(root == root_expected);
}
TEST(assets_load_unit_description_parser_test, single_section_single_entry) {
	load_unit_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test", {{"test1", ast::lookup_type::w, "test2"}}};
	root_expected.emplace_back(sec1);
	ASSERT_TRUE(root == root_expected);
}
TEST(assets_load_unit_description_parser_test, single_section_multi_entry) {
	load_unit_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	ast::load_unit_section sec1{"test",
								{{"test1", ast::lookup_type::w, "test2"},
								 {"test3", ast::lookup_type::w, "test4"},
								 {"test5", ast::lookup_type::w, "test6"}}};
	root_expected.emplace_back(sec1);
	ASSERT_TRUE(root == root_expected);
}
TEST(assets_load_unit_description_parser_test, multi_section_single_entry) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";}testB{\"test1\"->\"test2\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA", {{"test1", ast::lookup_type::w, "test2"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB", {{"test1", ast::lookup_type::w, "test2"}}});
	ASSERT_TRUE(root == root_expected);
}
TEST(assets_load_unit_description_parser_test, multi_section_multi_entry) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"d->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"d->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::d, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::d, "test6"}}});
	ASSERT_TRUE(root == root_expected);
}

TEST(assets_load_unit_description_parser_test, no_internal_path) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\";\"test3\"D;\"test5\";}"
						   "testB{\"test1\";\"test3\";\"test5\"d;}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, {}},
													   {"test3", ast::lookup_type::d, {}},
													   {"test5", ast::lookup_type::w, {}}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, {}},
													   {"test3", ast::lookup_type::w, {}},
													   {"test5", ast::lookup_type::d, {}}}});
	ASSERT_TRUE(root == root_expected);
}

TEST(assets_load_unit_description_parser_test, whitespace_skipping) {
	load_unit_description_parser parser;
	std::string testdata =
			"testA\n{\"test1\"\n->\n\"test2\";\"test3\" -> \"test4\"\n;\"test5\"\t-> \"test6\";}"
			"testB{\n\"test1\" ->\n\"test2\";\"test3\"\t->\t\"test4\"   ;\n    \"test5\"   ->    "
			"\"test6\"//test\n;}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unit test]", first, last);
	ASSERT_TRUE(first == last);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	ASSERT_TRUE(root == root_expected);
}

TEST(assets_load_unit_description_parser_test, syntax_error_missing_brace) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB \"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::load_unit_ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
}
TEST(assets_load_unit_description_parser_test, syntax_error_missing_quote) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB{test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::load_unit_ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
}
TEST(assets_load_unit_description_parser_test, syntax_error_missing_semicolon) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"->\"test2\"\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::load_unit_ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
}
TEST(assets_load_unit_description_parser_test, syntax_error_missing_arrow) {
	load_unit_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::load_unit_ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
	ast::load_unit_ast_root root_expected;
	root_expected.emplace_back(ast::load_unit_section{"testA",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
	root_expected.emplace_back(ast::load_unit_section{"testB",
													  {{"test1", ast::lookup_type::w, "test2"},
													   {"test3", ast::lookup_type::w, "test4"},
													   {"test5", ast::lookup_type::w, "test6"}}});
}

} // namespace parser
} // namespace asset_gen
} // namespace mce
