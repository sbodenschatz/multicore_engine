/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/ring_chunk_placer.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_RING_CHUNK_PLACER_HPP_
#define MCE_UTIL_RING_CHUNK_PLACER_HPP_

/**
 * \file
 * Defines the ring_chunk_placer class.
 */

#include <cstddef>
#include <tuple>

namespace mce {
namespace util {

/// Provides functionality to manage POD-chunks in a ring buffer (without owning the buffer).
class ring_chunk_placer {
	void* buffer_space_;
	size_t buffer_space_size_;
	size_t out_pos_ = 0;
	size_t in_pos_ = 0;
	size_t wrap_size_;

	std::tuple<void*, bool> find_pos(size_t data_size, size_t alignment);

public:
	/// Creates a rin_chunk_placer working on a buffer at the given address with the given size.
	ring_chunk_placer(void* buffer_space, size_t buffer_space_size);
	/// \brief Copies the given data into the ring buffer with the given alignment and returns the address of
	/// the copy or nullptr if there was insufficient space.
	void* place_chunk(const void* data, size_t data_size, size_t alignment = 1);
	/// Frees all data between the last freed address (initially start of the buffer) and the given address.
	void free_to(const void* end_of_space_to_free);
	/// Checks if a block of the given size and alignment can currently fit into the buffer.
	bool can_fit(size_t data_size, size_t alignment = 1);
	/// \brief Checks if a block of the given size and alignment can currently fit into the buffer without
	/// wrapping over the end.
	bool can_fit_no_wrap(size_t data_size, size_t alignment = 1);

	/// Returns the specified size of the buffer on which the ring_chunk_placer operates.
	size_t buffer_space_size() const {
		return buffer_space_size_;
	}
	/// Returns the address after the last inserted chunk (i.e. the "write" position).
	/**
	 * This member function is useful to obtain the the address to pass to free_to to free all previously
	 * placed chunks.
	 */
	const void* in_position() const {
		return static_cast<const char*>(buffer_space_) + in_pos_;
	}

	/// Converts the given pointer in the buffer space to the corresponding offset into the buffer space.
	size_t to_offset(void* ptr) const {
		return static_cast<const char*>(ptr) - static_cast<const char*>(buffer_space_);
	}

	/// Returns the space available without wrapping.
	size_t available_space_no_wrap() const {
		return ((out_pos_ <= in_pos_) ? buffer_space_size_ : (out_pos_ - 1)) - in_pos_;
	}

	/// Returns the space available in the buffer.
	size_t available_space() const {
		if(out_pos_ <= in_pos_) {
			return (buffer_space_size_ - in_pos_) + ((out_pos_ > 0) ? out_pos_ - 1 : 0);
		} else {
			return out_pos_ - 1 - in_pos_;
		}
	}
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_RING_CHUNK_PLACER_HPP_ */
