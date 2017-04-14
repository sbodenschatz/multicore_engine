/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/string_tools.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <boost/utility/string_view_fwd.hpp>
#include <util/string_tools.hpp>
#include <algorithm>
#include <cctype>

namespace mce {
namespace util {

bool ends_with(boost::string_view str, boost::string_view ending) {
	if(str.size() < ending.size()) return false;
	return std::equal(str.end() - ending.size(), str.end(), ending.begin(), ending.end());
}

bool ends_with_ignore_case(boost::string_view str, boost::string_view ending) {
	if(str.size() < ending.size()) return false;
	return std::equal(str.end() - ending.size(), str.end(), ending.begin(), ending.end(),
					  [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
}

bool starts_with(boost::string_view str, boost::string_view prefix) {
	if(str.size() < prefix.size()) return false;
	return std::equal(str.begin(), str.begin() + prefix.size(), prefix.begin(), prefix.end());
}

bool starts_with_ignore_case(boost::string_view str, boost::string_view prefix) {
	if(str.size() < prefix.size()) return false;
	return std::equal(str.begin(), str.begin() + prefix.size(), prefix.begin(), prefix.end(),
					  [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
}

} /* namespace util */
} /* namespace mce */
