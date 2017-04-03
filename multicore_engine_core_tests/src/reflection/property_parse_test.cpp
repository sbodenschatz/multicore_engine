/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/reflection/property_parse_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <reflection/property.hpp>

namespace mce {
namespace reflection {

BOOST_AUTO_TEST_SUITE(reflection)
BOOST_AUTO_TEST_SUITE(property_parse_test)

BOOST_AUTO_TEST_CASE(parse_int1) {
	struct test_class {
		int x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "12345";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x, 12345);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_int2) {
	struct test_class {
		glm::ivec2 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "12345 67890";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x.x, 12345);
	BOOST_CHECK_EQUAL(y.x.y, 67890);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_int3) {
	struct test_class {
		glm::ivec3 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "123 456 789";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x.x, 123);
	BOOST_CHECK_EQUAL(y.x.y, 456);
	BOOST_CHECK_EQUAL(y.x.z, 789);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_int4) {
	struct test_class {
		glm::ivec4 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "12 34 56 78";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x.x, 12);
	BOOST_CHECK_EQUAL(y.x.y, 34);
	BOOST_CHECK_EQUAL(y.x.z, 56);
	BOOST_CHECK_EQUAL(y.x.w, 78);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_float1) {
	struct test_class {
		float x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "123.45";
	prop->from_string(y, s_in);
	BOOST_CHECK_CLOSE(y.x, 123.45f, 0.00001f);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_float2) {
	struct test_class {
		glm::vec2 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "123.45 678.9";
	prop->from_string(y, s_in);
	BOOST_CHECK_CLOSE(y.x.x, 123.45f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.y, 678.9f, 0.00001f);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_float3) {
	struct test_class {
		glm::vec3 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "12.3 45.6 78.9";
	prop->from_string(y, s_in);
	BOOST_CHECK_CLOSE(y.x.x, 12.3f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.y, 45.6f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.z, 78.9f, 0.00001f);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_float4) {
	struct test_class {
		glm::vec4 x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "1.2 3.4 5.6 7.8";
	prop->from_string(y, s_in);
	BOOST_CHECK_CLOSE(y.x.x, 1.2f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.y, 3.4f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.z, 5.6f, 0.00001f);
	BOOST_CHECK_CLOSE(y.x.w, 7.8f, 0.00001f);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_string) {
	struct test_class {
		std::string x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "Hello World";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x, s_in);
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_CASE(parse_string_list) {
	struct test_class {
		std::vector<std::string> x;
	};
	auto prop = make_property<test_class>("x", &test_class::x);
	test_class y;
	std::string s_in = "Hello;World";
	prop->from_string(y, s_in);
	BOOST_CHECK_EQUAL(y.x.size(), 2);
	BOOST_CHECK_EQUAL(y.x.at(0), "Hello");
	BOOST_CHECK_EQUAL(y.x.at(1), "World");
	auto s_out = prop->to_string(y);
	BOOST_CHECK_EQUAL(s_in, s_out);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace reflection
} // namespace mce
