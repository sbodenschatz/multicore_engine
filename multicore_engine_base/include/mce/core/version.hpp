/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/core/version.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef CORE_VERSION_HPP_
#define CORE_VERSION_HPP_

/**
 * \file
 * Provides automatic version information for the engine and tools.
 */

#include <ostream>
#include <string>

namespace mce {
namespace core {

/// Returns a version string containing the version and branch from which this binary was built.
std::string get_build_version_string();
/// Returns the code version from which this binary was built.
std::string get_build_version();
/// Returns the branch from which this binary was built.
std::string get_build_branch();

/// Represents a version number of the engine.
struct version {
	int major;			///< The major version number (X in VX.Y.Z).
	int minor;			///< The minor version number (Y in VX.Y.Z).
	int patch;			///< The patch version number (Z in VX.Y.Z).
	int offset;			///< The number of commits after the tagged version.
	std::string commit; ///< The id of the commit.
};

/// Allows printing of version number structs.
inline std::ostream& operator<<(std::ostream& str, const version& v) {
	return str << v.major << "." << v.minor << "." << v.patch << "-" << v.offset << "-g" << v.commit;
}

/// Returns the version number of the code from which this binary was built.
version get_build_version_number();

/// Parses a string with version data from get_version_info in CMakeUtils.txt into a version struct.
version parse_version_string(const std::string& version_string);

/// Describes metadata about a piece of software (e.g. the engine or the application using it).
struct software_metadata {
	std::string name;	  ///< Name of the software.
	core::version version; ///< The referenced version of the software.
};

} // namespace core
} // namespace mce

#endif /* CORE_VERSION_HPP_ */
