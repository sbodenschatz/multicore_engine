/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/asset/pack_file_description_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <asset_gen/pack_file_description_parser.hpp>
#include <asset_gen/pack_file_description_ast.hpp>
#include <asset_gen/pack_file_description_ast_compare.hpp>

namespace mce {
namespace asset_gen {
namespace parser {

BOOST_AUTO_TEST_SUITE(assets)
BOOST_AUTO_TEST_SUITE(pack_file_description_parser_test)

BOOST_AUTO_TEST_CASE(empty_file_valid) {
	pack_file_description_parser parser;
	std::string testdata = "";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}
BOOST_AUTO_TEST_CASE(only_comment_valid) {
	pack_file_description_parser parser;
	std::string testdata = "//Comment\n";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}

BOOST_AUTO_TEST_CASE(single_empty_section) {
	pack_file_description_parser parser;
	std::string testdata = "test{}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	ast::pack_file_section sec1{"test", false, {}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_comment_in_section) {
	pack_file_description_parser parser;
	std::string testdata = "test{//Comment\n}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	ast::pack_file_section sec1{"test", false, {}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(single_section_single_entry) {
	pack_file_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	ast::pack_file_section sec1{"test", false, {{"test1", "test2"}}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(single_section_multi_entry) {
	pack_file_description_parser parser;
	std::string testdata = "test{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	ast::pack_file_section sec1{"test", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}};
	root_expected.emplace_back(sec1);
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(multi_section_single_entry) {
	pack_file_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";}testB{\"test1\"->\"test2\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(ast::pack_file_section{"testA", false, {{"test1", "test2"}}});
	root_expected.emplace_back(ast::pack_file_section{"testB", false, {{"test1", "test2"}}});
	BOOST_CHECK(root == root_expected);
}
BOOST_AUTO_TEST_CASE(multi_section_multi_entry) {
	pack_file_description_parser parser;
	std::string testdata = "testA{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(ast::pack_file_section{
			"testA", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	root_expected.emplace_back(ast::pack_file_section{
			"testB", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(no_internal_path) {
	pack_file_description_parser parser;
	std::string testdata = "testA{\"test1\";\"test3\";\"test5\";}"
						   "testB{\"test1\";\"test3\";\"test5\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(
			ast::pack_file_section{"testA", false, {{"test1", {}}, {"test3", {}}, {"test5", {}}}});
	root_expected.emplace_back(
			ast::pack_file_section{"testB", false, {{"test1", {}}, {"test3", {}}, {"test5", {}}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(multi_section_multi_entry_zip) {
	pack_file_description_parser parser;
	std::string testdata = "testA zip{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}"
						   "testB{\"test1\"->\"test2\";\"test3\"->\"test4\";\"test5\"->\"test6\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(ast::pack_file_section{
			"testA", true, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	root_expected.emplace_back(ast::pack_file_section{
			"testB", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(no_internal_path_zip) {
	pack_file_description_parser parser;
	std::string testdata = "testA zip{\"test1\";\"test3\";\"test5\";}"
						   "testB zip{\"test1\";\"test3\";\"test5\";}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(
			ast::pack_file_section{"testA", true, {{"test1", {}}, {"test3", {}}, {"test5", {}}}});
	root_expected.emplace_back(
			ast::pack_file_section{"testB", true, {{"test1", {}}, {"test3", {}}, {"test5", {}}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(whitespace_skipping) {
	pack_file_description_parser parser;
	std::string testdata =
			"testA\n{\"test1\"\n->\n\"test2\";\"test3\" -> \"test4\"\n;\"test5\"\t-> \"test6\";}"
			"testB{\n\"test1\" ->\n\"test2\";\"test3\"\t->\t\"test4\"   ;\n    \"test5\"   ->    "
			"\"test6\"//test\n;}";
	ast::pack_file_ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::pack_file_ast_root root_expected;
	root_expected.emplace_back(ast::pack_file_section{
			"testA", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	root_expected.emplace_back(ast::pack_file_section{
			"testB", false, {{"test1", "test2"}, {"test3", "test4"}, {"test5", "test6"}}});
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace parser
} // namespace asset_gen
} // namespace mce
