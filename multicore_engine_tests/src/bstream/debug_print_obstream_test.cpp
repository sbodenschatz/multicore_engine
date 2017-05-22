/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <mce/bstream/debug_print_obstream.hpp>
#include <gtest.hpp>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace mce {
namespace bstream {

struct bstream_debug_print_obstream_test : public ::testing::Test {
	std::stringstream output;
	debug_print_obstream stream{output};
};

// bstream_debug_print_obstream_test

TEST_F(bstream_debug_print_obstream_test, write_int8) {
	int8_t val1 = 0x42;
	int8_t val2 = -1;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 42 ]\n[ FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_uint8) {
	uint8_t val1 = 0x42;
	uint8_t val2 = 0xFF;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 42 ]\n[ FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_int16) {
	int16_t val1 = 0x4224;
	int16_t val2 = -1;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 24 42 ]\n[ FF FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_uint16) {
	uint16_t val1 = 0x4224;
	uint16_t val2 = 0xFFFF;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 24 42 ]\n[ FF FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_int32) {
	int32_t val1 = 0x76543210;
	int32_t val2 = -1;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 10 32 54 76 ]\n[ FF FF FF FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_uint32) {
	uint32_t val1 = 0x76543210;
	uint32_t val2 = 0xDEADBEEF;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ 10 32 54 76 ]\n[ EF BE AD DE ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_int64) {
	int64_t val1 = 0x0123456789ABCDEF;
	int64_t val2 = -1;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ EF CD AB 89 67 45 23 01 ]\n[ FF FF FF FF FF FF FF FF ]\n");
}

TEST_F(bstream_debug_print_obstream_test, write_uint64) {
	uint64_t val1 = 0x0123456789ABCDEF;
	uint64_t val2 = 0xDEADBEEF01234567;

	stream << val1 << val2;
	ASSERT_TRUE(output.str() == "[ EF CD AB 89 67 45 23 01 ]\n[ 67 45 23 01 EF BE AD DE ]\n");
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

TEST_F(bstream_debug_print_obstream_test, write_float) {
	float val = 42.123456f;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	ASSERT_TRUE(output.str() == chkstr);
}

TEST_F(bstream_debug_print_obstream_test, write_double) {
	double val = 42.123456;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	ASSERT_TRUE(output.str() == chkstr);
}

TEST_F(bstream_debug_print_obstream_test, write_long_double) {
	long double val = 42.123456;

	stream << val;

	unsigned char chk[sizeof(val)];
	memcpy(chk, &val, sizeof(val));

	std::string chkstr = "[ ";
	for(size_t i = 0; i < sizeof(val); ++i) {
		chkstr += to_hex(chk[i]) + " ";
	}
	chkstr += "]\n";
	ASSERT_TRUE(output.str() == chkstr);
}

TEST_F(bstream_debug_print_obstream_test, write_string) {
	std::string text = "Hello World!";
	stream << text;
	ASSERT_TRUE(output.str() == "[ 0C 00 00 00 00 00 00 00 ]"
								"\n[ 48 ]\n[ 65 ]\n[ 6C ]\n[ 6C ]\n[ 6F ]\n[ 20 ]"
								"\n[ 57 ]\n[ 6F ]\n[ 72 ]\n[ 6C ]\n[ 64 ]\n[ 21 ]\n");
}

TEST_F(bstream_debug_print_obstream_test, tell_and_size_after_write) {
	std::string text = "Hello World!";
	stream << text;
	ASSERT_TRUE(stream.size() == 20);
	ASSERT_TRUE(stream.tell_write() == 20);
}

TEST_F(bstream_debug_print_obstream_test, tell_and_size_after_write_and_seek) {
	std::string text = "Hello World!";
	stream << text;
	stream.seek_write(10);
	ASSERT_TRUE(stream.size() == 20);
	ASSERT_TRUE(stream.tell_write() == 10);
	stream.seek_write(20);
	ASSERT_TRUE(stream.size() == 20);
	ASSERT_TRUE(stream.tell_write() == 20);
	stream.seek_write(30);
	ASSERT_TRUE(stream.size() == 20);
	ASSERT_TRUE(stream.tell_write() == 20);
}

TEST_F(bstream_debug_print_obstream_test, seek_logging) {
	std::string text1 = "Hello World!";
	stream << text1;
	stream.seek_write(6);
	std::string text2 = "Test Test!";
	stream << text2;
	ASSERT_TRUE(stream.size() == 24);
	ASSERT_TRUE(stream.tell_write() == 24);

	ASSERT_TRUE(output.str() == "[ 0C 00 00 00 00 00 00 00 ]"
								"\n[ 48 ]\n[ 65 ]\n[ 6C ]\n[ 6C ]\n[ 6F ]\n[ 20 ]"
								"\n[ 57 ]\n[ 6F ]\n[ 72 ]\n[ 6C ]\n[ 64 ]\n[ 21 ]"
								"\n6:"
								"\n[ 0A 00 00 00 00 00 00 00 ]"
								"\n[ 54 ]\n[ 65 ]\n[ 73 ]\n[ 74 ]\n[ 20 ]\n[ 54 ]"
								"\n[ 65 ]\n[ 73 ]\n[ 74 ]\n[ 21 ]\n");
}

struct bstream_debug_print_obstream_int_test : public ::testing::Test {
	std::stringstream output;
	debug_print_obstream stream{output};
	bstream_debug_print_obstream_int_test() {
		stream.disable_print_bytes();
		stream.enable_print_integer_signed();
	}
};

// bstream_debug_print_obstream_int_test

TEST_F(bstream_debug_print_obstream_int_test, write_int8) {
	int8_t val1 = 0x42;
	int8_t val2 = -1;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 42_s8 >\n< -1_s8 >\n");
}

TEST_F(bstream_debug_print_obstream_int_test, write_int16) {
	int16_t val1 = 0x4224;
	int16_t val2 = -1;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 4224_s16 >< 24_s8 42_s8 >\n< -1_s16 >< -1_s8 -1_s8 >\n");
}

TEST_F(bstream_debug_print_obstream_int_test, write_int32) {
	int32_t val1 = 0x76543210;
	int32_t val2 = -1;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 76543210_s32 >< 3210_s16 7654_s16 >< 10_s8 32_s8 54_s8 76_s8 >"
							"\n< -1_s32 >< -1_s16 -1_s16 >< -1_s8 -1_s8 -1_s8 -1_s8 >\n");
}

TEST_F(bstream_debug_print_obstream_int_test, write_int64) {
	int64_t val1 = 0x0123456879ABCDEF;
	int64_t val2 = -1;

	stream << val1 << val2;
	ASSERT_EQ(output.str(),
			  "< 123456879ABCDEF_s64 >< 79ABCDEF_s32 1234568_s32 >"
			  "< -3211_s16 79AB_s16 4568_s16 123_s16 >< -11_s8 -33_s8 -55_s8 79_s8 68_s8 45_s8 23_s8 1_s8 >\n"
			  "< -1_s64 >< -1_s32 -1_s32 >< -1_s16 -1_s16 -1_s16 -1_s16 >"
			  "< -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 -1_s8 >\n");
}

struct bstream_debug_print_obstream_uint_test : public ::testing::Test {
	std::stringstream output;
	debug_print_obstream stream{output};
	bstream_debug_print_obstream_uint_test() {
		stream.disable_print_bytes();
		stream.enable_print_integer_unsigned();
	}
};

// bstream_debug_print_obstream_uint_test

TEST_F(bstream_debug_print_obstream_uint_test, write_uint8) {
	uint8_t val1 = 0x42;
	uint8_t val2 = 0xFF;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 42_u8 >\n< FF_u8 >\n");
}

TEST_F(bstream_debug_print_obstream_uint_test, write_uint16) {
	uint16_t val1 = 0x4224;
	uint16_t val2 = 0xFFFF;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 4224_u16 >< 24_u8 42_u8 >\n< FFFF_u16 >< FF_u8 FF_u8 >\n");
}

TEST_F(bstream_debug_print_obstream_uint_test, write_uint32) {
	uint32_t val1 = 0x76543210;
	uint32_t val2 = 0xDEADBEEF;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 76543210_u32 >< 3210_u16 7654_u16 >< 10_u8 32_u8 54_u8 76_u8 >\n"
							"< DEADBEEF_u32 >< BEEF_u16 DEAD_u16 >< EF_u8 BE_u8 AD_u8 DE_u8 >\n");
}

TEST_F(bstream_debug_print_obstream_uint_test, write_uint64) {
	uint64_t val1 = 0x0123456789ABCDEF;
	uint64_t val2 = 0xDEADBEEF01234567;

	stream << val1 << val2;
	ASSERT_EQ(output.str(), "< 123456789ABCDEF_u64 >< 89ABCDEF_u32 1234567_u32 >"
							"< CDEF_u16 89AB_u16 4567_u16 123_u16 >"
							"< EF_u8 CD_u8 AB_u8 89_u8 67_u8 45_u8 23_u8 1_u8 >\n"
							"< DEADBEEF01234567_u64 >< 1234567_u32 DEADBEEF_u32 >"
							"< 4567_u16 123_u16 BEEF_u16 DEAD_u16 >"
							"< 67_u8 45_u8 23_u8 1_u8 EF_u8 BE_u8 AD_u8 DE_u8 >\n");
}

struct bstream_debug_print_obstream_fp_char_test : public ::testing::Test {
	std::stringstream output;
	debug_print_obstream stream{output};
	bstream_debug_print_obstream_fp_char_test() {
		stream.disable_print_bytes();
		output << std::setprecision(8);
	}
};

// bstream_debug_print_obstream_fp_char_test

TEST_F(bstream_debug_print_obstream_fp_char_test, write_float) {
	float val = 42.123455f;
	stream.enable_print_float();
	stream << val;
	ASSERT_EQ(output.str(), "{ 42.123455f }\n");
}

TEST_F(bstream_debug_print_obstream_fp_char_test, write_double) {
	double val = 42.123456;
	stream.enable_print_double();
	stream << val;
	ASSERT_EQ(output.str(), "{ 42.123456d }\n");
}

TEST_F(bstream_debug_print_obstream_fp_char_test, write_long_double) {
	long double val = 42.123456;
	stream.enable_print_long_double();
	stream << val;
	ASSERT_EQ(output.str(), "{ 42.123456ld }\n");
}

TEST_F(bstream_debug_print_obstream_fp_char_test, write_string) {
	std::string text = "Hello World!";
	stream.enable_print_char();
	stream << text;
	ASSERT_EQ(output.str(), "( C 0 0 0 0 0 0 0 )\n"
							"( 'H' )\n( 'e' )\n( 'l' )\n( 'l' )\n( 'o' )\n( ' ' )\n"
							"( 'W' )\n( 'o' )\n( 'r' )\n( 'l' )\n( 'd' )\n( '!' )\n");
}

} /* namespace bstream */
} /* namespace mce */
