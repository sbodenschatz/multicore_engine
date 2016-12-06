/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/entity/entity_text_file_parser_frontend_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <boost/variant/get.hpp>
#include <entity/parser/entity_text_file_ast.hpp>
#include <entity/parser/entity_text_file_ast_compare.hpp>
#include <entity/parser/entity_text_file_parser.hpp>
#include <string>

namespace mce {
namespace entity {
namespace parser {

BOOST_AUTO_TEST_SUITE(entity)
BOOST_AUTO_TEST_SUITE(entity_text_file_parser_frontend_test)

BOOST_AUTO_TEST_CASE(empty_file_valid) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}
BOOST_AUTO_TEST_CASE(only_comment_valid) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "//Comment\n";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	BOOST_CHECK(root.size() == 0);
}

BOOST_AUTO_TEST_CASE(single_instance_unnamed_intlist_rotlist) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "Test (1,2,3),(x:45,y:30);";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::entity_instance ei;
	ei.type_name = "Test";
	ei.position_parameter = ast::int_list{1, 2, 3};
	ei.orientation_parameter = ast::rotation_list({ast::rotation_element{ast::rotation_axis::x, 45},
												   ast::rotation_element{ast::rotation_axis::y, 30}});
	root_expected.emplace_back(ei);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_instance_named_floatlist_floatlist) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "Test test(1,2,3.0),(1.0,2.0,3.0,4.0);";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::entity_instance ei;
	ei.type_name = "Test";
	ei.instance_name = "test";
	ei.position_parameter = ast::float_list{1, 2, 3.0};
	ei.orientation_parameter = ast::float_list{1.0, 2.0, 3.0, 4.0};
	root_expected.emplace_back(ei);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_include) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "include \"testfile.etf\";";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::include_instruction ii;
	ii.filename = "testfile.etf";
	root_expected.emplace_back(ii);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_empty) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	root_expected.emplace_back(ed);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_empty) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::entity_definition ed;
	ed.name = "TestEnt";
	ast::component_definition cd;
	cd.name = "TestComp";
	ed.components.emplace_back(cd);
	root_expected.emplace_back(ed);
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_intvar) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=42;}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_floatvar) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=42.0;}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_stringvar) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=\"Hello World!!\";}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_intlist) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(1,2,3);test2=();}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_floatlist) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(1,2,3.0);}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_stringlist) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=(\"Hello\",\"World\");}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_entityref) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=entity testent;}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_nosuper_comp_marker) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt{TestComp{test=marker \"testmarker\";}}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(single_definition_super_compreplace_comp) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "TestEnt:SuperEnt{"
						   "replace TestComp{test=(\"Hello\",\"World\");}"
						   "TestComp2{test=42;}"
						   "}";
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_CASE(combined) {
	entity_text_file_parser_frontend parser;
	std::string testdata = "include \"testfile.etf\";\n"
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
	ast::ast_root root;
	const char* first = testdata.data();
	const char* last = testdata.data() + testdata.size();
	bool result = parser.parse(first, last, root);
	BOOST_CHECK(result);
	BOOST_CHECK(first == last);
	ast::ast_root root_expected;
	ast::include_instruction ii;
	ii.filename = "testfile.etf";
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
	BOOST_CHECK(root == root_expected);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace parser
} // namespace entity
} // namespace mce
