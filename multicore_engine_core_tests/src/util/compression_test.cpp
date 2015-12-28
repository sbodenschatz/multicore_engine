/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/compression_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <util/compression.hpp>
#include <util/unused.hpp>
#include <algorithm>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(compression)

BOOST_AUTO_TEST_CASE(compression_decompression_level_0) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 0);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_1) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 1);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_2) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 2);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_3) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 3);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_4) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 4);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_5) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 5);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_6) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 6);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_7) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 7);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_8) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 8);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_CASE(compression_decompression_level_9) {
	std::vector<char> input = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
	auto compressed = compress(input, 9);
	auto decompressed = decompress(compressed);
	BOOST_CHECK(std::equal(input.begin(), input.end(), decompressed.begin(), decompressed.end()));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace util
} // namespace mce
