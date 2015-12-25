/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/compression.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <util/compression.hpp>
#include <zlib.h>
#include <util/unused.hpp>

namespace mce {
namespace util {

std::vector<char> compress(const std::vector<char>& input, int level) {
	return compress(input, level, std::vector<char>());
}
std::vector<char> compress(const std::vector<char>& input, int level, std::vector<char>&& buffer) {
	UNUSED(input);
	UNUSED(level);
	UNUSED(buffer);
	return std::vector<char>();
}
std::vector<char> decompress(const std::vector<char>& input) {
	return decompress(input, std::vector<char>());
}
std::vector<char> decompress(const std::vector<char>& input, std::vector<char>&& buffer) {
	UNUSED(input);
	UNUSED(buffer);
	return std::vector<char>();
}

} // namespace util
} // namespace mce
