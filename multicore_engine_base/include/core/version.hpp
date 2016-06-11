/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/core/version.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef CORE_VERSION_HPP_
#define CORE_VERSION_HPP_

#include <string>

namespace mce {
namespace core {

std::string get_build_version_string();
std::string get_build_version();
std::string get_build_branch();

} // namespace core
} // namespace mce

#endif /* CORE_VERSION_HPP_ */
