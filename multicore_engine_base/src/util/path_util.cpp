/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/path_util.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <util/path_util.hpp>

namespace mce {
namespace util {

void sanitize_path_inplace(std::string& path) noexcept {
	std::replace(path.begin(), path.end(), '\\', '/');
	std::unique(path.begin(), path.end(), [](auto x, auto y) { return x == y && x == '/'; });
#ifdef MULTICORE_ENGINE_WINDOWS
	std::replace(path.begin(), path.end(), '/', '\\');
#endif // MULTICORE_ENGINE_WINDOWS
}

} // namespace util
} // namespace mce
