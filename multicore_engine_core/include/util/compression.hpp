/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/compression.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_COMPRESSION_HPP_
#define UTIL_COMPRESSION_HPP_

#include <vector>

namespace mce {
namespace util {

std::vector<char> compress(const std::vector<char>& input, int level = -1);
std::vector<char> compress(const std::vector<char>& input, int level, std::vector<char>&& buffer);
std::vector<char> decompress(const std::vector<char>& input);
std::vector<char> decompress(const std::vector<char>& input, std::vector<char>&& buffer);

} // namespace util
} // namespace mce

#endif /* UTIL_COMPRESSION_HPP_ */
