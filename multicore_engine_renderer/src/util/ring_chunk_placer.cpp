/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/util/ring_chunk_placer.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cstring>
#include <mce/util/ring_chunk_placer.hpp>

namespace mce {
namespace util {

ring_chunk_placer::ring_chunk_placer(void* buffer_space, size_t buffer_space_size)
		: buffer_space_{buffer_space}, buffer_space_size_{buffer_space_size} {}
void* ring_chunk_placer::place_chunk(const void* data, size_t data_size) {
	if(out_pos_ <= in_pos_ && in_pos_ + data_size <= buffer_space_size_) {
		auto target = static_cast<char*>(buffer_space_) + in_pos_;
		in_pos_ += data_size;
		memcpy(target, data, data_size);
		return target;
	} else if(out_pos_ <= in_pos_) {
		in_pos_ = 0;
	}

	if(in_pos_ + data_size < out_pos_) {
		auto target = static_cast<char*>(buffer_space_) + in_pos_;
		in_pos_ += data_size;
		memcpy(target, data, data_size);
		return target;
	} else {
		// Doesn't fit.
		return nullptr;
	}
}
void ring_chunk_placer::free_to(size_t end_of_space_to_free) {
	out_pos_ = end_of_space_to_free;
}

} // namespace util
} // namespace mce
