/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/util/string_tools.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/utility/string_view_fwd.hpp>
#include <cctype>
#include <mce/util/string_tools.hpp>

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
bool equal_ignore_case(boost::string_view str_a, boost::string_view str_b) {
	if(str_a.size() != str_b.size()) return false;
	return std::equal(str_a.begin(), str_a.end(), str_b.begin(), str_b.end(),
					  [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
}

boost::string_view trim_left(boost::string_view str) {
	auto pos = str.find_first_not_of(" \t\r\n");
	if(pos == str.npos) return {};
	str.remove_prefix(pos);
	return str;
}
boost::string_view trim_right(boost::string_view str) {
	auto pos = str.find_last_not_of(" \t\r\n");
	if(pos == str.npos) return {};
	str.remove_suffix(str.size() - (pos + 1));
	return str;
}
boost::string_view trim(boost::string_view str) {
	return trim_left(trim_right(str));
}

} /* namespace util */
} /* namespace mce */
