/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/memory/align.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef MEMORY_ALIGN_HPP_
#define MEMORY_ALIGN_HPP_

namespace mce {
namespace memory {

// Implements the same task as std::align to compensate for lacking compiler support
void* align(std::size_t alignment, std::size_t size, void*& ptr, std::size_t& space);

} // namespace memory
} // namespace mce

#endif /* MEMORY_ALIGN_HPP_ */
