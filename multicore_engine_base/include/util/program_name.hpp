/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/program_name.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_PROGRAM_NAME_HPP_
#define UTIL_PROGRAM_NAME_HPP_

/**
 * \file
 * Defines helper functions to obtain the program name for tools.
 */

#include <string>

namespace mce {
namespace util {

/// Calculates the program name (without path or suffix) from the given first argument of main.
std::string calculate_program_name(const std::string& arg0);

} // namespace util
} // namespace mce

#endif /* UTIL_PROGRAM_NAME_HPP_ */
