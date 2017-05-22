/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/path_util.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_PATH_UTIL_HPP_
#define UTIL_PATH_UTIL_HPP_

/**
 * \file
 * Defines helper functions to work with paths.
 */

#include <string>

namespace mce {
namespace util {

/// Sanitizes the given part in place for the right type of slashes.
void sanitize_path_inplace(std::string& path) noexcept;

} // namespace util
} // namespace mce

#endif /* UTIL_PATH_UTIL_HPP_ */
