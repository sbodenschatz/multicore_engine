/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <bstream/debug_print_obstream.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
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

BOOST_AUTO_TEST_CASE(tell_and_size_after_write) {
	std::string text = "Hello World!";
	stream << text;
	BOOST_CHECK(stream.size() == 20);
	BOOST_CHECK(stream.tell_write() == 20);
}

BOOST_AUTO_TEST_CASE(tell_and_size_after_write_and_seek) {
	std::string text = "Hello World!";
	stream << text;
	stream.seek_write(10);
	BOOST_CHECK(stream.size() == 20);
	BOOST_CHECK(stream.tell_write() == 10);
	stream.seek_write(20);
	BOOST_CHECK(stream.size() == 20);
	BOOST_CHECK(stream.tell_write() == 20);
	stream.seek_write(30);
	BOOST_CHECK(stream.size() == 20);
	BOOST_CHECK(stream.tell_write() == 20);
}

BOOST_AUTO_TEST_CASE(seek_logging) {
	std::string text1 = "Hello World!";
	stream << text1;
	stream.seek_write(6);
	std::string text2 = "Test Test!";
	stream << text2;
	BOOST_CHECK(stream.size() == 24);
	BOOST_CHECK(stream.tell_write() == 24);

	BOOST_CHECK(output.str() == "[ 0C 00 00 00 00 00 00 00 ]"
								"\n[ 48 ]\n[ 65 ]\n[ 6C ]\n[ 6C ]\n[ 6F ]\n[ 20 ]"
								"\n[ 57 ]\n[ 6F ]\n[ 72 ]\n[ 6C ]\n[ 64 ]\n[ 21 ]"
								"\n6:"
								"\n[ 0A 00 00 00 00 00 00 00 ]"
								"\n[ 54 ]\n[ 65 ]\n[ 73 ]\n[ 74 ]\n[ 20 ]\n[ 54 ]"
								"\n[ 65 ]\n[ 73 ]\n[ 74 ]\n[ 21 ]\n");
}

BOOST_AUTO_TEST_SUITE_END()

struct debug_print_obstream_int_fixture {
	std::stringstream output;
	debug_print_obstream stream{output};
	debug_print_obstream_int_fixture() {
		stream.disable_print_bytes();
		stream.enable_print_integer_signed();
	}
};

BOOST_FIXTURE_TEST_SUITE(debug_print_obstream_int_test, debug_print_obstream_int_fixture)
BOOST_AUTO_TEST_CASE(write_int8) {
	int8_t val1 = 0x42;
	int8_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 42_s8 >\n< -1_s8 >\n");
}

BOOST_AUTO_TEST_CASE(write_int16) {
	int16_t val1 = 0x4224;
	int16_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 4224_s16 >< 24_s8 42_s8 >\n< -1_s16 >< -1_s8 -1_s8 >\n");
}

BOOST_AUTO_TEST_CASE(write_int32) {
	int32_t val1 = 0x76543210;
	int32_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 76543210_s32 >< 3210_s16 7654_s16 >< 10_s8 32_s8 54_s8 76_s8 >"
									"\n< -1_s32 >< -1_s16 -1_s16 >< -1_s8 -1_s8 -1_s8 -1_s8 >\n");
}

BOOST_AUTO_TEST_CASE(write_int64) {
	int64_t val1 = 0x0123456879ABCDEF;
	int64_t val2 = -1;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(
			output.str(),
			"< 123456879ABCDEF_s64 >< 79ABCDEF_s32 1234568_s32 >"
			"< -3211_s16 79AB_s16 4568_s16 123_s16 >< -11_s8 -33_s8 -55_s8 79_s8 68_s8 45_s8 23_s8 1_s8 >\n"
			"< -1_s64 >< -1_s32 -1_s32 >< -1_s16 -1_s16 -1_s16 -1_s16 >"
			"< -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 >\n");
}
BOOST_AUTO_TEST_SUITE_END()

struct debug_print_obstream_uint_fixture {
	std::stringstream output;
	debug_print_obstream stream{output};
	debug_print_obstream_uint_fixture() {
		stream.disable_print_bytes();
		stream.enable_print_integer_unsigned();
	}
};

BOOST_FIXTURE_TEST_SUITE(debug_print_obstream_uint_test, debug_print_obstream_uint_fixture)

BOOST_AUTO_TEST_CASE(write_uint8) {
	uint8_t val1 = 0x42;
	uint8_t val2 = 0xFF;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 42_u8 >\n< FF_u8 >\n");
}

BOOST_AUTO_TEST_CASE(write_uint16) {
	uint16_t val1 = 0x4224;
	uint16_t val2 = 0xFFFF;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 4224_u16 >< 24_u8 42_u8 >\n< FFFF_u16 >< FF_u8 FF_u8 >\n");
}

BOOST_AUTO_TEST_CASE(write_uint32) {
	uint32_t val1 = 0x76543210;
	uint32_t val2 = 0xDEADBEEF;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 76543210_u32 >< 3210_u16 7654_u16 >< 10_u8 32_u8 54_u8 76_u8 >\n"
									"< DEADBEEF_u32 >< BEEF_u16 DEAD_u16 >< EF_u8 BE_u8 AD_u8 DE_u8 >\n");
}

BOOST_AUTO_TEST_CASE(write_uint64) {
	uint64_t val1 = 0x0123456789ABCDEF;
	uint64_t val2 = 0xDEADBEEF01234567;

	stream << val1 << val2;
	BOOST_CHECK_EQUAL(output.str(), "< 123456789ABCDEF_u64 >< 89ABCDEF_u32 1234567_u32 >"
									"< CDEF_u16 89AB_u16 4567_u16 123_u16 >"
									"< EF_u8 CD_u8 AB_u8 89_u8 67_u8 45_u8 23_u8 1_u8 >\n"
									"< DEADBEEF01234567_u64 >< 1234567_u32 DEADBEEF_u32 >"
									"< 4567_u16 123_u16 BEEF_u16 DEAD_u16 >"
									"< 67_u8 45_u8 23_u8 1_u8 EF_u8 BE_u8 AD_u8 DE_u8 >\n");
}

BOOST_AUTO_TEST_SUITE_END()

struct debug_print_obstream_fp_fixture {
	std::stringstream output;
	debug_print_obstream stream{output};
	debug_print_obstream_fp_fixture() {
		stream.disable_print_bytes();
		output << std::setprecision(8);
	}
};

BOOST_FIXTURE_TEST_SUITE(debug_print_obstream_fp_char_test, debug_print_obstream_fp_fixture)

BOOST_AUTO_TEST_CASE(write_float) {
	float val = 42.123455f;
	stream.enable_print_float();
	stream << val;
	BOOST_CHECK_EQUAL(output.str(), "{ 42.123455f }\n");
}

BOOST_AUTO_TEST_CASE(write_double) {
	double val = 42.123456;
	stream.enable_print_double();
	stream << val;
	BOOST_CHECK_EQUAL(output.str(), "{ 42.123456d }\n");
}

BOOST_AUTO_TEST_CASE(write_long_double) {
	long double val = 42.123456;
	stream.enable_print_long_double();
	stream << val;
	BOOST_CHECK_EQUAL(output.str(), "{ 42.123456ld }\n");
}

BOOST_AUTO_TEST_CASE(write_string) {
	std::string text = "Hello World!";
	stream.enable_print_char();
	stream << text;
	BOOST_CHECK_EQUAL(output.str(), "( C 0 0 0 0 0 0 0 )\n"
									"( 'H' )\n( 'e' )\n( 'l' )\n( 'l' )\n( 'o' )\n( ' ' )\n"
									"( 'W' )\n( 'o' )\n( 'r' )\n( 'l' )\n( 'd' )\n( '!' )\n");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace bstream */
} /* namespace mce */
