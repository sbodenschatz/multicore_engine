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
	dstr.str("test.name=Default Value\n");
	std::stringstream ustr;
	ustr.str("test.name=Test String\n");
	config_store cs(ustr, dstr, [](config_store::config_storer&) {});
	auto test_name = cs.resolve<std::string>("test.name");
	ASSERT_EQ("Test String", test_name->value());
}

} // namespace config
} // namespace mce
