/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/file_reader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <asset/file_reader.hpp>

namespace mce {
namespace asset {

void file_reader::sanitize_path_inplace(std::string& path) noexcept {
	std::replace(path.begin(), path.end(), '\\', '/');
	std::unique(path.begin(), path.end(), [](auto x, auto y) { return x == y && x == '/'; });
#ifdef MULTICORE_ENGINE_WINDOWS
	std::replace(path.begin(), path.end(), '/', '\\');
#endif // MULTICORE_ENGINE_WINDOWS
}

} // namespace asset
} // namespace mce
