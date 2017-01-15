/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/path_util.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_PATH_UTIL_HPP_
#define UTIL_PATH_UTIL_HPP_

#include <string>

namespace mce {
namespace util {

/// Sanitizes the given part in place for the right type of slashes.
void sanitize_path_inplace(std::string& path) noexcept;

} // namespace util
} // namespace mce

#endif /* UTIL_PATH_UTIL_HPP_ */
