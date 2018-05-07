/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/util/error_helper.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_ERROR_HELPER_HPP_
#define UTIL_ERROR_HELPER_HPP_

/**
 * \file
 * Provides error outputting helper functions for tools to format errors like IDEs expect them.
 */

#include <cstdint>
#include <mce/exceptions.hpp>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>

namespace mce {
namespace util {
namespace detail {

/// Formats an error position like the used build system expects them.
inline void print_error_position(std::ostream& ostr, const std::string& filename, size_t line,
								 size_t column) {
#ifdef _MSC_VER
	ostr << filename;
	ostr << "(";
	ostr << line;
	ostr << ",";
	ostr << column;
	ostr << ")";
#else
	ostr << filename;
	ostr << ":";
	ostr << line;
	ostr << ":";
	ostr << column;
#endif
}

} // namespace detail

/// Throws a syntax error exception for the given position in the given filename with the supplied message.
template <typename It>
void throw_syntax_error(const std::string& filename, It buffer_start, It pos, const std::string& message) {
	auto p = std::accumulate(buffer_start, pos, std::make_pair(size_t(1), size_t(1)),
							 [](std::pair<size_t, size_t> p, char c) {
								 return (c == '\n') ? std::make_pair(p.first + 1, size_t(1))
													: std::make_pair(p.first, p.second + 1);
							 });
	std::stringstream msgstream;
	detail::print_error_position(msgstream, filename, p.first, p.second);
	msgstream << ": error: ";
	msgstream << message;
	throw syntax_exception(msgstream.str());
}

/// \brief Throws a syntax error exception for the given position in the given filename with the supplied
/// message and expected element.
template <typename It, typename Expected>
void throw_syntax_error(const std::string& filename, It buffer_start, It pos, const std::string& message,
						const Expected& expected) {
	auto p = std::accumulate(buffer_start, pos, std::make_pair(size_t(1), size_t(1)),
							 [](std::pair<size_t, size_t> p, char c) {
								 return (c == '\n') ? std::make_pair(p.first + 1, size_t(1))
													: std::make_pair(p.first, p.second + 1);
							 });
	std::stringstream msgstream;
	detail::print_error_position(msgstream, filename, p.first, p.second);
	msgstream << ": error: ";
	msgstream << message;
	msgstream << " - Expected " << expected;
	throw syntax_exception(msgstream.str());
}

} // namespace util
} // namespace mce

#endif /* UTIL_ERROR_HELPER_HPP_ */
