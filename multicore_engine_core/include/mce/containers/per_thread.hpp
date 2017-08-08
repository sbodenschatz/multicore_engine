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

template <typename T>
class per_thread {
	size_t total_slots_;
	std::atomic<size_t> used_slots_;
	dynamic_array<std::atomic<std::thread::id>> owners_;
	dynamic_array<T> values_;

public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	template <typename... Args>
	per_thread(size_type slots, Args&&... args)
			: total_slots_{slots}, used_slots_{0}, owners_(slots, std::thread::id()),
			  values_(slots, std::forward<Args>(args)...) {}

	per_thread(const per_thread&) = delete;
	per_thread& operator=(const per_thread&) = delete;
	per_thread(per_thread&&) = delete;
	per_thread& operator=(per_thread&&) = delete;

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
	reference get() {
		return values_[slot_index()];
	}

	iterator begin() noexcept {
		return values_.begin();
	}
	const_iterator begin() const noexcept {
		return values_.begin();
	}
	const_iterator cbegin() const noexcept {
		return values_.begin();
	}
	iterator end() noexcept {
		return values_.begin() + used_slots_.load();
	}
	const_iterator end() const noexcept {
		return values_.begin() + used_slots_.load();
	}
	const_iterator cend() const noexcept {
		return values_.begin() + used_slots_.load();
	}
	reverse_iterator rbegin() noexcept {
		return end();
	}
	const_reverse_iterator rbegin() const noexcept {
		return end();
	}
	const_reverse_iterator crbegin() const noexcept {
		return end();
	}
	reverse_iterator rend() noexcept {
		return begin();
	}
	const_reverse_iterator rend() const noexcept {
		return begin();
	}
	const_reverse_iterator crend() const {
		return begin();
	}

	size_type total_slots() const {
		return total_slots_;
	}

	size_type used_slots() const {
		return used_slots_.load();
	}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_PER_THREAD_HPP_ */
