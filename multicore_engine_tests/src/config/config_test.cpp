/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/config/config_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <config/config_store.hpp>
#include <gtest.hpp>
#include <sstream>

namespace mce {
namespace config {

TEST(config_config_store, load_config_strings) {
	std::stringstream dstr;
	dstr.str("test.name1=Default Value\n"
			 "test.name2=Default 2\n");
	std::stringstream ustr;
	ustr.str("test.name1=Test String\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<std::string>("test.name1");
	ASSERT_EQ("Test String", test_name1->value());
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<std::string>("test.name2");
	ASSERT_EQ("Default 2", test_name2->value());
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_int) {
	std::stringstream dstr;
	dstr.str("test.name1=12345\n"
			 "test.name2=6789\n");
	std::stringstream ustr;
	ustr.str("test.name1=13579\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<int>("test.name1");
	ASSERT_EQ(13579, test_name1->value());
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<int>("test.name2");
	ASSERT_EQ(6789, test_name2->value());
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_float) {
	std::stringstream dstr;
	dstr.str("test.name1=123.45\n"
			 "test.name2=67.89\n");
	std::stringstream ustr;
	ustr.str("test.name1=135.79\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<float>("test.name1");
	ASSERT_FLOAT_EQ(135.79f, test_name1->value());
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<float>("test.name2");
	ASSERT_FLOAT_EQ(67.89f, test_name2->value());
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_string_list) {
	std::stringstream dstr;
	dstr.str("test.name1=Hello;World\n"
			 "test.name2=Test1;Test2;Test3\n");
	std::stringstream ustr;
	ustr.str("test.name1=Testing;String;Lists\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<std::vector<std::string>>("test.name1");
	auto test_name1_expected = std::vector<std::string>{"Testing", "String", "Lists"};
	ASSERT_EQ(test_name1_expected, test_name1->value());
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<std::vector<std::string>>("test.name2");
	auto test_name2_expected = std::vector<std::string>{"Test1", "Test2", "Test3"};
	ASSERT_EQ(test_name2_expected, test_name2->value());
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_vec2) {
	std::stringstream dstr;
	dstr.str("test.name1=123.45 234.56\n"
			 "test.name2=67.89 78.90\n");
	std::stringstream ustr;
	ustr.str("test.name1=135.79 246.80\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::vec2>("test.name1");
	ASSERT_FLOAT_EQ(135.79f, test_name1->value().x);
	ASSERT_FLOAT_EQ(246.80f, test_name1->value().y);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::vec2>("test.name2");
	ASSERT_FLOAT_EQ(67.89f, test_name2->value().x);
	ASSERT_FLOAT_EQ(78.90f, test_name2->value().y);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_vec3) {
	std::stringstream dstr;
	dstr.str("test.name1=123.45 234.56 345.67\n"
			 "test.name2=67.89 78.90 89.01\n");
	std::stringstream ustr;
	ustr.str("test.name1=135.79 246.80 357.91\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::vec3>("test.name1");
	ASSERT_FLOAT_EQ(135.79f, test_name1->value().x);
	ASSERT_FLOAT_EQ(246.80f, test_name1->value().y);
	ASSERT_FLOAT_EQ(357.91f, test_name1->value().z);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::vec3>("test.name2");
	ASSERT_FLOAT_EQ(67.89f, test_name2->value().x);
	ASSERT_FLOAT_EQ(78.90f, test_name2->value().y);
	ASSERT_FLOAT_EQ(89.01f, test_name2->value().z);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_vec4) {
	std::stringstream dstr;
	dstr.str("test.name1=123.45 234.56 345.67 456.78\n"
			 "test.name2=67.89 78.90 89.01 90.12\n");
	std::stringstream ustr;
	ustr.str("test.name1=135.79 246.80 357.91 468.02\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::vec4>("test.name1");
	ASSERT_FLOAT_EQ(135.79f, test_name1->value().x);
	ASSERT_FLOAT_EQ(246.80f, test_name1->value().y);
	ASSERT_FLOAT_EQ(357.91f, test_name1->value().z);
	ASSERT_FLOAT_EQ(468.02f, test_name1->value().w);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::vec4>("test.name2");
	ASSERT_FLOAT_EQ(67.89f, test_name2->value().x);
	ASSERT_FLOAT_EQ(78.90f, test_name2->value().y);
	ASSERT_FLOAT_EQ(89.01f, test_name2->value().z);
	ASSERT_FLOAT_EQ(90.12f, test_name2->value().w);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_ivec2) {
	std::stringstream dstr;
	dstr.str("test.name1=12345 23456\n"
			 "test.name2=6789 7890\n");
	std::stringstream ustr;
	ustr.str("test.name1=13579 24680\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::ivec2>("test.name1");
	ASSERT_EQ(13579, test_name1->value().x);
	ASSERT_EQ(24680, test_name1->value().y);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::ivec2>("test.name2");
	ASSERT_EQ(6789, test_name2->value().x);
	ASSERT_EQ(7890, test_name2->value().y);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_ivec3) {
	std::stringstream dstr;
	dstr.str("test.name1=12345 23456 34567\n"
			 "test.name2=6789 7890 8901\n");
	std::stringstream ustr;
	ustr.str("test.name1=13579 24680 35791\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::ivec3>("test.name1");
	ASSERT_EQ(13579, test_name1->value().x);
	ASSERT_EQ(24680, test_name1->value().y);
	ASSERT_EQ(35791, test_name1->value().z);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::ivec3>("test.name2");
	ASSERT_EQ(6789, test_name2->value().x);
	ASSERT_EQ(7890, test_name2->value().y);
	ASSERT_EQ(8901, test_name2->value().z);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_config_ivec4) {
	std::stringstream dstr;
	dstr.str("test.name1=12345 23456 34567 45678\n"
			 "test.name2=6789 7890 8901 9012\n");
	std::stringstream ustr;
	ustr.str("test.name1=13579 24680 35791 46802\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name1 = cs.resolve<glm::ivec4>("test.name1");
	ASSERT_EQ(13579, test_name1->value().x);
	ASSERT_EQ(24680, test_name1->value().y);
	ASSERT_EQ(35791, test_name1->value().z);
	ASSERT_EQ(46802, test_name1->value().w);
	ASSERT_FALSE(test_name1->dirty());
	auto test_name2 = cs.resolve<glm::ivec4>("test.name2");
	ASSERT_EQ(6789, test_name2->value().x);
	ASSERT_EQ(7890, test_name2->value().y);
	ASSERT_EQ(8901, test_name2->value().z);
	ASSERT_EQ(9012, test_name2->value().w);
	ASSERT_FALSE(test_name2->dirty());
}

TEST(config_config_store, load_save_unmodified_load) {
	std::stringstream dstr;
	dstr.str("test1=123\n"
			 "number=12345\n"
			 "text=Hello World\n"
			 "test123=987.65 43.21\n"
			 "test=Test Test Test\n"
			 "vector=123 456 789\n"
			 "list=Hello;World\n");
	std::stringstream ustr;
	ustr.str("number=345\n"
			 "text=Hello Test\n"
			 "vector=12 45 78\n"
			 "list=Hello;Test;World\n"
			 "new.value=This was not set in default config\n");
	std::stringstream ostr;
	{
		config_store cs(ustr, dstr, [&](config_store::config_storer& s) { s.store(ostr); });
		auto text = cs.resolve<std::string>("text");
		ASSERT_EQ("Hello Test", text->value());
	}
	ASSERT_EQ(ustr.str(), ostr.str());
	config_store cs(ustr, ostr, [&](config_store::config_storer&) {});
	auto text = cs.resolve<std::string>("text");
	ASSERT_EQ("Hello Test", text->value());
	auto number = cs.resolve<int>("number");
	ASSERT_EQ(345, number->value());
	auto vector = cs.resolve<glm::ivec3>("vector");
	ASSERT_EQ(12, vector->value().x);
	ASSERT_EQ(45, vector->value().y);
	ASSERT_EQ(78, vector->value().z);
	auto list = cs.resolve<std::vector<std::string>>("list");
	auto list_expected = std::vector<std::string>{"Hello", "Test", "World"};
	ASSERT_EQ(list_expected, list->value());
	auto newvalue = cs.resolve<std::string>("new.value");
	ASSERT_EQ("This was not set in default config", newvalue->value());
}

TEST(config_config_store, load_modify_save_load) {
	std::stringstream dstr;
	dstr.str("test1=123\n"
			 "number=12345\n"
			 "text=Hello World\n"
			 "test123=987.65 43.21\n"
			 "test=Test Test Test\n"
			 "vector=123 456 789\n"
			 "list=Hello;World\n");
	std::stringstream ustr;
	ustr.str("number=345\n"
			 "text=Hello Test\n"
			 "vector=12 45 78\n"
			 "list=Hello;Test;World\n"
			 "new.value=This was not set in default config\n");
	std::stringstream ostr;
	int new_number = 54321;
	std::string new_text = "New Text";
	glm::ivec3 new_vector{1234, 5678, 9123};
	std::vector<std::string> new_list{"New", "List", "Value"};
	std::string new_newvalue = "This is the newly set text.";
	glm::vec4 new_vec4{123.4f, 567.8f, 912.3f, 456.7f};
	{
		config_store cs(ustr, dstr, [&](config_store::config_storer& s) { s.store(ostr); });
		auto number = cs.resolve<int>("number");
		number->value(new_number);
		auto text = cs.resolve<std::string>("text");
		text->value(new_text);
		auto vector = cs.resolve<glm::ivec3>("vector");
		vector->value(new_vector);
		auto list = cs.resolve<std::vector<std::string>>("list");
		list->value(new_list);
		auto newvalue = cs.resolve<std::string>("new.value");
		newvalue->value(new_newvalue);
		auto vec4 = cs.resolve<glm::vec4>("vec4");
		vec4->value(new_vec4);
	}
	ASSERT_EQ("number=54321\n"
			  "text=New Text\n"
			  "vector=1234 5678 9123\n"
			  "list=New;List;Value\n"
			  "new.value=This is the newly set text.\n"
			  "vec4=123.4 567.8 912.3 456.7\n",
			  ostr.str());
	config_store cs(ustr, ostr, [&](config_store::config_storer&) {});
	auto text = cs.resolve<std::string>("text");
	ASSERT_EQ(new_text, text->value());
	auto number = cs.resolve<int>("number");
	ASSERT_EQ(new_number, number->value());
	auto vector = cs.resolve<glm::ivec3>("vector");
	ASSERT_EQ(new_vector.x, vector->value().x);
	ASSERT_EQ(new_vector.y, vector->value().y);
	ASSERT_EQ(new_vector.z, vector->value().z);
	auto list = cs.resolve<std::vector<std::string>>("list");
	ASSERT_EQ(new_list, list->value());
	auto newvalue = cs.resolve<std::string>("new.value");
	ASSERT_EQ(new_newvalue, newvalue->value());
	auto vec4 = cs.resolve<glm::vec4>("vec4");
	ASSERT_FLOAT_EQ(new_vec4.x, vec4->value().x);
	ASSERT_FLOAT_EQ(new_vec4.y, vec4->value().y);
	ASSERT_FLOAT_EQ(new_vec4.z, vec4->value().z);
	ASSERT_FLOAT_EQ(new_vec4.w, vec4->value().w);
}

} // namespace config
} // namespace mce
