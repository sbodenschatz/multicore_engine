/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/string_tools.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_STRING_TOOLS_HPP_
#define UTIL_STRING_TOOLS_HPP_

#include <boost/utility/string_view.hpp>

namespace mce {
namespace util {

bool ends_with(boost::string_view str, boost::string_view ending);
bool ends_with_ignore_case(boost::string_view str, boost::string_view ending);
bool starts_with(boost::string_view str, boost::string_view prefix);
bool starts_with_ignore_case(boost::string_view str, boost::string_view prefix);

} /* namespace util */
} /* namespace mce */

#endif /* UTIL_STRING_TOOLS_HPP_ */
