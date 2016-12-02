/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/string_tools.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_STRING_TOOLS_HPP_
#define UTIL_STRING_TOOLS_HPP_

#include <string>

namespace mce {
namespace util {

bool ends_with(const std::string& str, const std::string& ending);
bool ends_with_ignore_case(const std::string& str, const std::string& ending);
bool starts_with(const std::string& str, const std::string& prefix);
bool starts_with_ignore_case(const std::string& str, const std::string& prefix);

} /* namespace util */
} /* namespace mce */

#endif /* UTIL_STRING_TOOLS_HPP_ */
