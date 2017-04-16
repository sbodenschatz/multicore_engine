/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/core/version.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef CORE_VERSION_HPP_
#define CORE_VERSION_HPP_

/**
 * \file
 * Provides automatic version information for the engine and tools.
 */

#include <string>

namespace mce {
namespace core {

/// Returns a version string containing the version and branch from which this binary was built.
std::string get_build_version_string();
/// Returns the code version from which this binary was built.
std::string get_build_version();
/// Returns the branch from which this binary was built.
std::string get_build_branch();

} // namespace core
} // namespace mce

#endif /* CORE_VERSION_HPP_ */
