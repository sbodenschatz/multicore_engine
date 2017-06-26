/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/util/ring_chunk_placer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_RING_CHUNK_PLACER_HPP_
#define MCE_UTIL_RING_CHUNK_PLACER_HPP_

#include <cstddef>
#include <tuple>

namespace mce {
namespace util {

class ring_chunk_placer {
	void* buffer_space_;
	size_t buffer_space_size_;
	size_t out_pos_ = 0;
	size_t in_pos_ = 0;
	size_t wrap_size_;

	std::tuple<void*, bool> find_pos(size_t data_size);

public:
	ring_chunk_placer(void* buffer_space, size_t buffer_space_size);
	void* place_chunk(const void* data, size_t data_size);
	void free_to(const void* end_of_space_to_free);
	bool can_fit(size_t data_size);
	bool can_fit_no_wrap(size_t data_size);
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_RING_CHUNK_PLACER_HPP_ */
