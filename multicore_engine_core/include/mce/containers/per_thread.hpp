/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/per_thread.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_PER_THREAD_HPP_
#define MCE_CONTAINERS_PER_THREAD_HPP_

#include <atomic>
#include <mce/containers/dynamic_array.hpp>
#include <mce/exceptions.hpp>
#include <thread>

namespace mce {
namespace containers {

class per_thread_index {
	size_t total_slots_;
	std::atomic<size_t> used_slots_;
	dynamic_array<std::atomic<std::thread::id>> owners_;

public:
	using size_type = std::size_t;

	per_thread_index(size_type slots)
			: total_slots_{slots}, used_slots_{0}, owners_(slots, std::thread::id()) {}

	per_thread_index(const per_thread_index&) = delete;
	per_thread_index& operator=(const per_thread_index&) = delete;
	per_thread_index(per_thread_index&&) = delete;
	per_thread_index& operator=(per_thread_index&&) = delete;

	size_type slot_index() {
		auto used = used_slots_.load();
		auto my_id = std::this_thread::get_id();
		for(size_type i = 0; i < used; ++i) {
			if(owners_[i].load() == my_id) return i;
		}
		auto my_index = used_slots_.load();
		do {
			if(my_index == total_slots_) {
				throw mce::resource_depleted_exception("No more slots available.");
			}
		} while(!used_slots_.compare_exchange_weak(my_index, my_index + 1));
		owners_[my_index] = my_id;
		return my_index;
	}

	size_type total_slots() const {
		return total_slots_;
	}

	size_type used_slots() const {
		return used_slots_.load();
	}
};

/// \brief Provides a fixed size pool of objects that are associated to using threads but can also be accessed
/// single-threaded (e.g. by a management thread)..
template <typename T>
class per_thread {
	per_thread_index index_mapping_;
	dynamic_array<T> values_;

public:
	/// The type of the values in the pool.
	using value_type = T;
	/// The type used for sizes and indices.
	using size_type = std::size_t;
	/// The type used for references to the elements.
	using reference = value_type&;
	/// The type used for read-only references to the elements.
	using const_reference = const value_type&;
	/// The type used for pointers to the elements.
	using pointer = value_type*;
	/// The type used for read-only pointers to the elements.
	using const_pointer = const value_type*;
	/// The type of read-write iterators over the elements.
	using iterator = pointer;
	/// The type of read-only iterators over the elements.
	using const_iterator = const_pointer;
	/// The type of read-write iterators over the elements with reversed traversal.
	using reverse_iterator = std::reverse_iterator<iterator>;
	/// The type of read-only iterators over the elements with reversed traversal.
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	/// \brief Creates a per_thread with the given number of slots for the threads initializing the objects
	/// using the given constructor parameters.
	/**
	 * Additionally to simply passing values, any of the parameters can be given as one of the following place
	 * holder tags:
	 * - index_param_tag<Integral> to pass the element index casted to Integral in it's place.
	 * - generator_param_tag<Functor> to pass in it's place the return value of calling operator()(size_t) of
	 * Functor with the element index. Such a generator_param tag can be created with template argument
	 * deduction using generator_param(F).
	 */
	template <typename... Args>
	per_thread(size_type slots, Args&&... args)
			: index_mapping_{slots}, values_(slots, std::forward<Args>(args)...) {}

	/// Forbids copying.
	per_thread(const per_thread&) = delete;
	/// Forbids copying.
	per_thread& operator=(const per_thread&) = delete;
	/// Forbids moving.
	per_thread(per_thread&&) = delete;
	/// Forbids moving.
	per_thread& operator=(per_thread&&) = delete;

	/// Looks up and returns the index for the calling thread.
	/**
	 * If there are no slots left and the thread has no associated slot yet, an exception of type
	 * mce::resource_depleted_exception is thrown.
	 */
	size_type slot_index() {
		return index_mapping_.slot_index();
	}
	/// Looks up the index for the calling thread and returns a reference to the associated object.
	/**
	 * If there are no slots left and the thread has no associated slot yet, an exception of type
	 * mce::resource_depleted_exception is thrown.
	 */
	reference get() {
		return values_[slot_index()];
	}

	/// Returns an read-write iterator referring to the beginning of the used part of the objects array.
	iterator begin() noexcept {
		return values_.begin();
	}
	/// Returns an read-only iterator referring to the beginning of the used part of the objects array.
	const_iterator begin() const noexcept {
		return values_.begin();
	}
	/// Returns an read-only iterator referring to the beginning of the used part of the objects array.
	const_iterator cbegin() const noexcept {
		return values_.begin();
	}
	/// Returns an read-write iterator referring to the end of the used part of the objects array.
	iterator end() noexcept {
		return values_.begin() + index_mapping_.used_slots();
	}
	/// Returns an read-only iterator referring to the end of the used part of the objects array.
	const_iterator end() const noexcept {
		return values_.begin() + index_mapping_.used_slots();
	}
	/// Returns an read-only iterator referring to the end of the used part of the objects array.
	const_iterator cend() const noexcept {
		return values_.begin() + index_mapping_.used_slots();
	}
	/// \brief Returns an read-write iterator referring to the beginning of the used part of the objects array
	/// in reverse order.
	reverse_iterator rbegin() noexcept {
		return end();
	}
	/// \brief Returns an read-only iterator referring to the beginning of the used part of the objects array
	/// in reverse order.
	const_reverse_iterator rbegin() const noexcept {
		return end();
	}
	/// \brief Returns an read-only iterator referring to the beginning of the used part of the objects array
	/// in reverse order.
	const_reverse_iterator crbegin() const noexcept {
		return end();
	}
	/// \brief Returns an read-write iterator referring to the end of the used part of the objects array in
	/// reverse order.
	reverse_iterator rend() noexcept {
		return begin();
	}
	/// \brief Returns an read-only iterator referring to the end of the used part of the objects array in
	/// reverse order.
	const_reverse_iterator rend() const noexcept {
		return begin();
	}
	/// \brief Returns an read-only iterator referring to the end of the used part of the objects array in
	/// reverse order.
	const_reverse_iterator crend() const {
		return begin();
	}

	/// Returns the total number of slots in the pool.
	size_type total_slots() const {
		return index_mapping_.total_slots();
	}

	/// Returns the number of slots used by / assigned to a thread.
	size_type used_slots() const {
		return index_mapping_.used_slots();
	}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_PER_THREAD_HPP_ */
