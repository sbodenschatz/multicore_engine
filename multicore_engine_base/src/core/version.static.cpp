/*
 * Multi-Core Engine project
 * File /multicore_engine_base/src/core/version.static.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <core/version.hpp>
#include <sstream>

namespace mce {
namespace core {

version get_build_version_number() {
	version v;
	std::istringstream str(get_build_version());
	char skip;
	str >> skip >> v.major;
	str >> skip >> v.minor;
	str >> skip >> v.patch;
	str >> skip >> v.offset;
	str >> skip;
	str >> skip >> v.commit;
	return v;
}

} // namespace core
} // namespace mce
