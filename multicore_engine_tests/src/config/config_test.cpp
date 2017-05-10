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

TEST(config_config_store, load_config) {
	std::stringstream dstr;
	dstr.str("test.name1=Default Value\n"
			 "test.name2=Default 2");
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

} // namespace config
} // namespace mce
