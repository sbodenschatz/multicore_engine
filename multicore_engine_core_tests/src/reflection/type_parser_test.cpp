/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/reflection/type_parser_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <reflection/type.hpp>

namespace mce {
namespace reflection {

BOOST_AUTO_TEST_SUITE(reflection)
BOOST_AUTO_TEST_SUITE(type_parser_test)

BOOST_AUTO_TEST_CASE(parse_int1) {
	int res = 0;
	type_parser<decltype(res)>::from_string("12345", res);
	BOOST_CHECK(res == 12345);
}

BOOST_AUTO_TEST_CASE(parse_int2) {
	glm::ivec2 res{0, 0};
	type_parser<decltype(res)>::from_string("1234 5678", res);
	BOOST_CHECK(res.x == 1234);
	BOOST_CHECK(res.y == 5678);
}

BOOST_AUTO_TEST_CASE(parse_int3) {
	glm::ivec3 res{0, 0, 0};
	type_parser<decltype(res)>::from_string("123 456 789", res);
	BOOST_CHECK(res.x == 123);
	BOOST_CHECK(res.y == 456);
	BOOST_CHECK(res.z == 789);
}

BOOST_AUTO_TEST_CASE(parse_int4) {
	glm::ivec4 res{0, 0, 0, 0};
	type_parser<decltype(res)>::from_string("12 34 56 78", res);
	BOOST_CHECK(res.x == 12);
	BOOST_CHECK(res.y == 34);
	BOOST_CHECK(res.z == 56);
	BOOST_CHECK(res.w == 78);
}

BOOST_AUTO_TEST_CASE(parse_float1) {
	float res = 0;
	type_parser<decltype(res)>::from_string("123.45", res);
	BOOST_CHECK_CLOSE(res, 123.45f, 0.00001f);
}

BOOST_AUTO_TEST_CASE(parse_float2) {
	glm::vec2 res{0, 0};
	type_parser<decltype(res)>::from_string("12.34 56.78", res);
	BOOST_CHECK_CLOSE(res.x, 12.34f, 0.00001f);
	BOOST_CHECK_CLOSE(res.y, 56.78f, 0.00001f);
}

BOOST_AUTO_TEST_CASE(parse_float3) {
	glm::vec3 res{0, 0, 0};
	type_parser<decltype(res)>::from_string("12.3 45.6 78.9", res);
	BOOST_CHECK_CLOSE(res.x, 12.3f, 0.00001f);
	BOOST_CHECK_CLOSE(res.y, 45.6f, 0.00001f);
	BOOST_CHECK_CLOSE(res.z, 78.9f, 0.00001f);
}

BOOST_AUTO_TEST_CASE(parse_float4) {
	glm::vec4 res{0, 0, 0, 0};
	type_parser<decltype(res)>::from_string("1.2 3.4 5.6 7.8", res);
	BOOST_CHECK_CLOSE(res.x, 1.2f, 0.00001f);
	BOOST_CHECK_CLOSE(res.y, 3.4f, 0.00001f);
	BOOST_CHECK_CLOSE(res.z, 5.6f, 0.00001f);
	BOOST_CHECK_CLOSE(res.w, 7.8f, 0.00001f);
}

BOOST_AUTO_TEST_CASE(parse_string) {
	std::string res;
	type_parser<decltype(res)>::from_string("Hello World", res);
	BOOST_CHECK_EQUAL(res, "Hello World");
}

BOOST_AUTO_TEST_CASE(parse_string_list) {
	std::vector<std::string> res;
	type_parser<decltype(res)>::from_string("Hello;World", res);
	BOOST_CHECK_EQUAL(res.size(), 2);
	BOOST_CHECK_EQUAL(res.at(0), "Hello");
	BOOST_CHECK_EQUAL(res.at(1), "World");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace reflection
} // namespace mce
