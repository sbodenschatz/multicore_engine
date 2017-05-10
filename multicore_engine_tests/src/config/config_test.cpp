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

} // namespace config
} // namespace mce
