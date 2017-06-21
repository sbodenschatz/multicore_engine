/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/util/ring_chunk_placer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_RING_CHUNK_PLACER_HPP_
#define MCE_UTIL_RING_CHUNK_PLACER_HPP_

#include <cstddef>

namespace mce {
namespace util {

class ring_chunk_placer {
	void* buffer_space_;
	size_t buffer_space_size_;
	size_t out_pos_ = 0;
	size_t in_pos_ = 0;
	size_t wrap_size_;

public:
	ring_chunk_placer(void* buffer_space, size_t buffer_space_size);
	void* place_chunk(const void* data, size_t data_size);
	void free_to(size_t end_of_space_to_free);
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_RING_CHUNK_PLACER_HPP_ */
