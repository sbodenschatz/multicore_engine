/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <bstream/debug_print_obstream.hpp>
#include <iostream>
#include <sstream>

namespace mce {
namespace bstream {

struct debug_print_obstream_fixture {
	std::stringstream output;
	debug_print_obstream stream{output};
};

BOOST_AUTO_TEST_SUITE(bstream)
BOOST_FIXTURE_TEST_SUITE(debug_print_obstream_test, debug_print_obstream_fixture)
BOOST_AUTO_TEST_CASE(write_int8) {
	int8_t val1 = 0x42;
	int8_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 42 ]\n[ FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_uint8) {
	uint8_t val1 = 0x42;
	uint8_t val2 = 0xFF;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 42 ]\n[ FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_int16) {
	int16_t val1 = 0x4224;
	int16_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 24 42 ]\n[ FF FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_uint16) {
	uint16_t val1 = 0x4224;
	uint16_t val2 = 0xFFFF;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 24 42 ]\n[ FF FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_int32) {
	int32_t val1 = 0x76543210;
	int32_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 10 32 54 76 ]\n[ FF FF FF FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_uint32) {
	uint32_t val1 = 0x76543210;
	uint32_t val2 = 0xDEADBEEF;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ 10 32 54 76 ]\n[ EF BE AD DE ]\n");
}

BOOST_AUTO_TEST_CASE(write_int64) {
	int64_t val1 = 0x0123456789ABCDEF;
	int64_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ EF CD AB 89 67 45 23 01 ]\n[ FF FF FF FF FF FF FF FF ]\n");
}

BOOST_AUTO_TEST_CASE(write_uint64) {
	uint64_t val1 = 0x0123456789ABCDEF;
	uint64_t val2 = 0xDEADBEEF01234567;

	stream << val1 << val2;
	BOOST_CHECK(output.str() == "[ EF CD AB 89 67 45 23 01 ]\n[ 67 45 23 01 EF BE AD DE ]\n");
}

static std::string to_hex(unsigned char val) {
	char digits[2] = {char(val / 0x10), char(val % 0x10)};
	for(size_t i = 0; i < 2; ++i) {
		if(digits[i] > 9) {
			digits[i] -= 10;
			digits[i] += 'A';
		} else {
			digits[i] += '0';
		}
	}
	return std::string(digits, 2);
}

BOOST_AUTO_TEST_CASE(write_float) {
	float val = 42.123456f;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	BOOST_CHECK(output.str() == chkstr);
}

BOOST_AUTO_TEST_CASE(write_double) {
	double val = 42.123456;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	BOOST_CHECK(output.str() == chkstr);
}

BOOST_AUTO_TEST_CASE(write_long_double) {
	long double val = 42.123456;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	BOOST_CHECK(output.str() == chkstr);
}

BOOST_AUTO_TEST_CASE(write_string) {
	std::string text = "Hello World!";
	stream << text;
	BOOST_CHECK(output.str() == "[ 0C 00 00 00 00 00 00 00 ]"
								"\n[ 48 ]\n[ 65 ]\n[ 6C ]\n[ 6C ]\n[ 6F ]\n[ 20 ]"
								"\n[ 57 ]\n[ 6F ]\n[ 72 ]\n[ 6C ]\n[ 64 ]\n[ 21 ]\n");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace bstream */
} /* namespace mce */
