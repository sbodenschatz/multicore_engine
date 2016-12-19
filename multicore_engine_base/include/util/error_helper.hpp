/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/error_helper.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_ERROR_HELPER_HPP_
#define UTIL_ERROR_HELPER_HPP_

#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>

namespace mce {
namespace util {
namespace detail {

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
	throw std::runtime_error(msgstream.str());
}

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
	throw std::runtime_error(msgstream.str());
}

} // namespace util
} // namespace mce

#endif /* UTIL_ERROR_HELPER_HPP_ */
