/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/memory/align.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef MEMORY_ALIGN_HPP_
#define MEMORY_ALIGN_HPP_

#include <cstddef>

namespace mce {
namespace memory {

// Implements the same task as std::align to compensate for lacking compiler support
void* align(std::size_t alignment, std::size_t size, void*& ptr, std::size_t& space);

template <typename T>
bool align_offset(T alignment, T size, T& ptr, T& space) {
	T unalignment = ptr & (alignment - 1);
	T offset = T(0);
	if(unalignment) {
		offset = alignment - unalignment;
	}
	if(size + offset > space) return false;
	ptr += offset;
	space -= offset;
	return true;
}

} // namespace memory
} // namespace mce

#endif /* MEMORY_ALIGN_HPP_ */
