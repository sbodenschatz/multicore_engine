/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/ring_chunk_placer.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cassert>
#include <cstring>
#include <mce/memory/align.hpp>
#include <mce/util/ring_chunk_placer.hpp>

namespace mce {
namespace util {

ring_chunk_placer::ring_chunk_placer(void* buffer_space, size_t buffer_space_size)
		: buffer_space_{buffer_space}, buffer_space_size_{buffer_space_size}, wrap_size_{buffer_space_size_} {
}
std::tuple<void*, bool> ring_chunk_placer::find_pos(size_t data_size, size_t alignment) {
	auto in_pos = in_pos_;
	bool wrap = false;
	if(out_pos_ <= in_pos && in_pos + data_size <= buffer_space_size_) {
		auto in_pos_aligned = in_pos;
		auto space = buffer_space_size_ - in_pos;
		//(out_pos_ - in_pos - 1);
		if(memory::align_offset(alignment, data_size, in_pos_aligned, space)) {
			return std::make_tuple(static_cast<char*>(buffer_space_) + in_pos_aligned, wrap);
		}
	} else if(out_pos_ <= in_pos) {
		in_pos = 0;
		wrap = true;
	}

	if(in_pos == out_pos_) {
		return std::make_tuple(nullptr, wrap);
	}
	auto in_pos_aligned = in_pos;
	auto space = out_pos_ - in_pos - 1;
	if(memory::align_offset(alignment, data_size, in_pos_aligned, space)) {
		return std::make_tuple(static_cast<char*>(buffer_space_) + in_pos_aligned, wrap);
	} else {
		return std::make_tuple(nullptr, wrap);
	}
}

void* ring_chunk_placer::place_chunk(const void* data, size_t data_size, size_t alignment) {
	void* target = nullptr;
	bool wrap = false;
	std::tie(target, wrap) = find_pos(data_size, alignment);
	if(!target) return nullptr;
	if(wrap) {
		wrap_size_ = in_pos_;
		in_pos_ = 0;
	}
	in_pos_ = (static_cast<char*>(target) - static_cast<char*>(buffer_space_)) + data_size;
	memcpy(target, data, data_size);
	return target;
}
void ring_chunk_placer::free_to(const void* end_of_space_to_free) {
	auto buffer_start = static_cast<const char*>(buffer_space_);
	auto free_end = static_cast<const char*>(end_of_space_to_free);
	if(free_end < buffer_start || free_end > (buffer_start + buffer_space_size_)) return;
	out_pos_ = free_end - buffer_start;
	assert(out_pos_ <= wrap_size_);
	if(out_pos_ == wrap_size_) out_pos_ = 0;
}
bool ring_chunk_placer::can_fit(size_t data_size, size_t alignment) {
	void* target = nullptr;
	bool wrap = false;
	std::tie(target, wrap) = find_pos(data_size, alignment);
	return target;
}
bool ring_chunk_placer::can_fit_no_wrap(size_t data_size, size_t alignment) {
	void* target = nullptr;
	bool wrap = false;
	std::tie(target, wrap) = find_pos(data_size, alignment);
	return target && !wrap;
}

} // namespace util
} // namespace mce
