/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/string_tools.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <cctype>
#include <util/string_tools.hpp>

namespace mce {
namespace util {

bool ends_with(const std::string& str, const std::string& ending) {
	if(str.size() < ending.size()) return false;
	return std::equal(str.end() - ending.size(), str.end(), ending.begin(), ending.end());
}

bool ends_with_ignore_case(const std::string& str, const std::string& ending) {
	if(str.size() < ending.size()) return false;
	return std::equal(str.end() - ending.size(), str.end(), ending.begin(), ending.end(),
					  [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
}

} /* namespace util */
} /* namespace mce */
