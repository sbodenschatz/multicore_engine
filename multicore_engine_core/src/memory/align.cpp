/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/memory/align.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <cstddef>
#include <cstdint>

namespace mce {
namespace memory {

// Implements the same task as std::align to compensate for lacking compiler support.
void* align(size_t alignment, size_t size, void*& ptr, size_t& space) {
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
