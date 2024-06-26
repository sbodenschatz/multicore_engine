/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/entity/entity_template_lang_parser_frontend_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/variant/get.hpp>
#include <gtest.hpp>
#include <mce/entity/parser/entity_template_lang_ast.hpp>
#include <mce/entity/parser/entity_template_lang_ast_compare.hpp>
#include <mce/entity/parser/entity_template_lang_parser.hpp>
#include <mce/exceptions.hpp>
#include <string>

namespace mce {
namespace entity {
namespace parser {

TEST(entity_entity_template_lang_parser_frontend_test, empty_file_valid) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ASSERT_TRUE(root.size() == 0);
}
TEST(entity_entity_template_lang_parser_frontend_test, only_comment_valid) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "//Comment\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ASSERT_TRUE(root.size() == 0);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_instance_unnamed_intlist_rotlist) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "Test (1,2,3),(x:45,y:30);";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_instance ei;
	ei.type_name = "Test";
	ei.position_parameter = ast::int_list{1, 2, 3};
	ei.orientation_parameter = ast::rotation_list({ast::rotation_element{ast::rotation_axis::x, 45},
												   ast::rotation_element{ast::rotation_axis::y, 30}});
	root_expected.emplace_back(ei);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_instance_named_floatlist_floatlist) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_instance ei;
	ei.type_name = "Test";
	ei.instance_name = "test";
	ei.position_parameter = ast::float_list{1, 2, 3.0};
	ei.orientation_parameter = ast::float_list{1.0, 2.0, 3.0, 4.0};
	root_expected.emplace_back(ei);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_include) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::include_instruction ii;
	ii.filename = "testfile.etl";
	root_expected.emplace_back(ii);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_empty) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_empty) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_intvar) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=42;}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = static_cast<long long>(42);
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_floatvar) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=42.0;}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = ast::float_node(42.0);
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_stringvar) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=\"Hello World!!\";}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = "Hello World!!";
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_intlist) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(1,2,3);test2=();}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	{
		ast::variable var;
		var.name = "test";
		var.value = ast::int_list{1, 2, 3};
		cd.variables.emplace_back(var);
	}
	{
		ast::variable var;
		var.name = "test2";
		var.value = ast::int_list{};
		cd.variables.emplace_back(var);
	}
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_floatlist) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(1,2,3.0);}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = ast::float_list{1.0f, 2.0f, 3.0f};
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_stringlist) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(\"Hello\",\"World\");}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = ast::string_list{"Hello", "World"};
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_entityref) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=entity testent;}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = ast::entity_reference{"testent"};
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_nosuper_comp_marker) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=marker \"testmarker\";}}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ast::variable var;
	var.name = "test";
	var.value = ast::marker_evaluation{"testmarker"};
	cd.variables.emplace_back(var);
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, single_definition_super_compreplace_comp) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "TestEnt:SuperEnt{"
						   "replace TestComp{test=(\"Hello\",\"World\");}"
						   "TestComp2{test=42;}"
						   "}";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ed.super_name = "SuperEnt";
	{
		ast::component_definition cd;
		cd.name = "TestComp";
		cd.replace = true;
		ast::variable var;
		var.name = "test";
		var.value = ast::string_list{"Hello", "World"};
		cd.variables.emplace_back(var);
		ed.components.emplace_back(cd);
	}
	{
		ast::component_definition cd;
		cd.name = "TestComp2";
		ast::variable var;
		var.name = "test";
		var.value = static_cast<long long>(42);
		cd.variables.emplace_back(var);
		ed.components.emplace_back(cd);
	}
	root_expected.emplace_back(ed);
	ASSERT_TRUE(root == root_expected);
}

TEST(entity_entity_template_lang_parser_frontend_test, combined) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	auto root = parser.parse("[unittest]", first, last);
	ASSERT_TRUE(first == last);
	ast::ast_root root_expected;
	ast::include_instruction ii;
	ii.filename = "testfile.etl";
	root_expected.emplace_back(ii);
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ed.super_name = "SuperEnt";
	{
		ast::component_definition cd;
		cd.name = "TestComp";
		cd.replace = true;
		ast::variable var;
		var.name = "test";
		var.value = ast::string_list{"Hello", "World"};
		cd.variables.emplace_back(var);
		ed.components.emplace_back(cd);
	}
	{
		ast::component_definition cd;
		cd.name = "TestComp2";
		ast::variable var;
		var.name = "test";
		var.value = static_cast<long long>(42);
		cd.variables.emplace_back(var);
		ed.components.emplace_back(cd);
	}
	root_expected.emplace_back(ed);
	{
		ast::entity_instance ei;
		ei.type_name = "Test";
		ei.position_parameter = ast::int_list{1, 2, 3};
		ei.orientation_parameter = ast::rotation_list({ast::rotation_element{ast::rotation_axis::x, 45},
													   ast::rotation_element{ast::rotation_axis::y, 30}});
		root_expected.emplace_back(ei);
	}
	{
		ast::entity_instance ei;
		ei.type_name = "Test";
		ei.instance_name = "test";
		ei.position_parameter = ast::float_list{1, 2, 3.0};
		ei.orientation_parameter = ast::float_list{1.0, 2.0, 3.0, 4.0};
		root_expected.emplace_back(ei);
	}
	ASSERT_TRUE(root == root_expected);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_include_non_string) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include testfile.etl;\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_super) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_template_name) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   ":SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_invalid_modifier) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	repl TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_comp_name) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_var_name) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_delimiter) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\"\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_value) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_missing_semicolon) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_instance_missing_delimiter) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3)(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_instance_missing_internal_delimiter) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30);\n"
						   "Test test(1,2,3.0),(1.02.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}
TEST(entity_entity_template_lang_parser_frontend_test, syntax_error_instance_missing_semicolon) {
	entity_template_lang_parser_frontend parser;
	std::string testdata = "include \"testfile.etl\";\n"
						   "//Comment\n"
						   "TestEnt:SuperEnt{\n"
						   "	replace TestComp{\n"
						   "		test=(\"Hello\",\"World\");\n"
						   "	}\n"
						   "	TestComp2{\n"
						   "		test=42;\n"
						   "	}\n"
						   "}\n"
						   "Test (1,2,3),(x:45,y:30)\n"
						   "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);\n";
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	ast::ast_root root;
	ASSERT_THROW(root = parser.parse("[unit test]", first, last), syntax_exception);
}

} // namespace parser
} // namespace entity
} // namespace mce
