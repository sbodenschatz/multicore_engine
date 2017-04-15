/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/memory/align.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <memory/align.hpp>
#include <cstddef>
#include <cstdint>

namespace mce {
namespace memory {

// Implements the same task as std::align to compensate for lacking compiler support.
void* align(std::size_t alignment, std::size_t size, void*& ptr, std::size_t& space) {
	char* tmp_ptr = reinterpret_cast<char*>(ptr);
	uintptr_t tmp_int = reinterpret_cast<uintptr_t>(tmp_ptr);
	size_t unalignment = tmp_int & (alignment - 1);
	size_t offset = 0;
	if(unalignment) {
		offset = alignment - unalignment;
	}
	if(size + offset > space) return nullptr;
	tmp_ptr += offset;
	space -= offset;
	return ptr = tmp_ptr;
}

} // namespace memory
} // namespace mce
