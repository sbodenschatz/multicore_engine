/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/memory/aligned.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MEMORY_ALIGNED_HPP_
#define MEMORY_ALIGNED_HPP_

#include <cstdint>

namespace mce {
namespace memory {

inline bool is_aligned(const void* addr, uintptr_t alignment) noexcept {
	uintptr_t addr_int = uintptr_t(addr);
	return addr_int % alignment == 0;
}

} // namespace memory
} // namespace mce

#endif /* MEMORY_ALIGNED_HPP_ */
