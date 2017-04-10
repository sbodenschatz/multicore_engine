/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/reflection/type_parser_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <reflection/type.hpp>

namespace mce {
namespace reflection {

TEST(reflection_type_parser_test, parse_int1) {
	int res = 0;
	type_parser<decltype(res)>::from_string("12345", res);
	ASSERT_TRUE(res == 12345);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "12345");
}

TEST(reflection_type_parser_test, parse_int2) {
	glm::ivec2 res{0, 0};
	type_parser<decltype(res)>::from_string("1234 5678", res);
	ASSERT_TRUE(res.x == 1234);
	ASSERT_TRUE(res.y == 5678);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "1234 5678");
}

TEST(reflection_type_parser_test, parse_int3) {
	glm::ivec3 res{0, 0, 0};
	type_parser<decltype(res)>::from_string("123 456 789", res);
	ASSERT_TRUE(res.x == 123);
	ASSERT_TRUE(res.y == 456);
	ASSERT_TRUE(res.z == 789);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "123 456 789");
}

TEST(reflection_type_parser_test, parse_int4) {
	glm::ivec4 res{0, 0, 0, 0};
	type_parser<decltype(res)>::from_string("12 34 56 78", res);
	ASSERT_TRUE(res.x == 12);
	ASSERT_TRUE(res.y == 34);
	ASSERT_TRUE(res.z == 56);
	ASSERT_TRUE(res.w == 78);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "12 34 56 78");
}

TEST(reflection_type_parser_test, parse_float1) {
	float res = 0;
	type_parser<decltype(res)>::from_string("123.45", res);
	ASSERT_NEAR(res, 123.45f, 0.00001f);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "123.45");
}

TEST(reflection_type_parser_test, parse_float2) {
	glm::vec2 res{0, 0};
	type_parser<decltype(res)>::from_string("12.34 56.78", res);
	ASSERT_NEAR(res.x, 12.34f, 0.00001f);
	ASSERT_NEAR(res.y, 56.78f, 0.00001f);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "12.34 56.78");
}

TEST(reflection_type_parser_test, parse_float3) {
	glm::vec3 res{0, 0, 0};
	type_parser<decltype(res)>::from_string("12.3 45.6 78.9", res);
	ASSERT_NEAR(res.x, 12.3f, 0.00001f);
	ASSERT_NEAR(res.y, 45.6f, 0.00001f);
	ASSERT_NEAR(res.z, 78.9f, 0.00001f);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "12.3 45.6 78.9");
}

TEST(reflection_type_parser_test, parse_float4) {
	glm::vec4 res{0, 0, 0, 0};
	type_parser<decltype(res)>::from_string("1.2 3.4 5.6 7.8", res);
	ASSERT_NEAR(res.x, 1.2f, 0.00001f);
	ASSERT_NEAR(res.y, 3.4f, 0.00001f);
	ASSERT_NEAR(res.z, 5.6f, 0.00001f);
	ASSERT_NEAR(res.w, 7.8f, 0.00001f);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "1.2 3.4 5.6 7.8");
}

TEST(reflection_type_parser_test, parse_quat) {
	glm::vec4 res{0, 0, 0, 0};
	type_parser<decltype(res)>::from_string("1.2 3.4 5.6 7.8", res);
	ASSERT_NEAR(res.x, 1.2f, 0.00001f);
	ASSERT_NEAR(res.y, 3.4f, 0.00001f);
	ASSERT_NEAR(res.z, 5.6f, 0.00001f);
	ASSERT_NEAR(res.w, 7.8f, 0.00001f);

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "1.2 3.4 5.6 7.8");
}

TEST(reflection_type_parser_test, parse_string) {
	std::string res;
	type_parser<decltype(res)>::from_string("Hello World", res);
	ASSERT_EQ(res, "Hello World");

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "Hello World");
}

TEST(reflection_type_parser_test, parse_string_list) {
	std::vector<std::string> res;
	type_parser<decltype(res)>::from_string("Hello;World", res);
	ASSERT_EQ(res.size(), 2);
	ASSERT_EQ(res.at(0), "Hello");
	ASSERT_EQ(res.at(1), "World");

	auto s = type_parser<decltype(res)>::to_string(res);
	ASSERT_EQ(s, "Hello;World");
}

} // namespace reflection
} // namespace mce
