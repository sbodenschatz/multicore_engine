/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/util/string_tools.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_STRING_TOOLS_HPP_
#define UTIL_STRING_TOOLS_HPP_

/**
 * \file
 * Provides various string helper functions.
 */

#include <boost/utility/string_view.hpp>

namespace mce {
namespace util {

/// Checks if the given string view ends with the given suffix.
bool ends_with(boost::string_view str, boost::string_view ending);
/// Checks if the given string view ends with the given suffix when character case is ignored.
bool ends_with_ignore_case(boost::string_view str, boost::string_view ending);
/// Checks if the given string view starts with the given prefix.
bool starts_with(boost::string_view str, boost::string_view prefix);
/// Checks if the given string view starts with the given prefix when character case is ignored.
bool starts_with_ignore_case(boost::string_view str, boost::string_view prefix);
/// Checks if the given string views are equal when character case is ignored.
bool equal_ignore_case(boost::string_view str_a, boost::string_view str_b);

/// Allows efficient processing of delimited strings in place using function objects.
/**
 * The function object f must be callable with a boost::string_view&& as a parameter.
 * It will be called for every substring in str when str is split with delim as a delimiter.
 */
template <typename F>
void split_iterate(boost::string_view str, boost::string_view delim, F f) {
	boost::string_view::size_type pos = 0;
	for(boost::string_view::size_type next = str.find(delim, pos); next != str.npos;) {
		f(str.substr(pos, next - pos));
		pos = next + 1;
		next = str.find(delim, pos);
	}
	f(str.substr(pos));
}

/// Removes white-space chars (space, tab, line break) before the first non-whitespace char.
boost::string_view trim_left(boost::string_view str);
/// Removes white-space chars (space, tab, line break) after the last non-whitespace char.
boost::string_view trim_right(boost::string_view str);
/// \brief Removes white-space chars (space, tab, line break) from both ends of the string so the result
/// starts and ends with non-whitespace chars.
boost::string_view trim(boost::string_view str);

} /* namespace util */
} /* namespace mce */

#endif /* UTIL_STRING_TOOLS_HPP_ */
