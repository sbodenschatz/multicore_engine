/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/copy_on_write.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_COPY_ON_WRITE_HPP_
#define UTIL_COPY_ON_WRITE_HPP_

#include "monitor.hpp"
#include <memory>

namespace mce {
namespace util {

template <typename T>
class copy_on_write {
	// TODO replace monitor<std::shared_ptr<T>> with std::atomic_shared_ptr<T> when available.
	monitor<std::shared_ptr<T>> ptr;

public:
	copy_on_write() : ptr{std::make_shared<T>()} {}
	copy_on_write(const std::shared_ptr<T>& object_to_manage) : ptr{object_to_manage} {}
	copy_on_write(std::shared_ptr<T>&& object_to_manage) : ptr{object_to_manage} {}
	copy_on_write(const copy_on_write&) = delete;
	copy_on_write& operator=(const copy_on_write&) = delete;

	std::shared_ptr<T> get() const {
		return ptr;
	}

	template <typename F>
	void do_transaction(F f) {
		auto old_ptr = ptr.load();
		auto new_ptr = std::make_shared<T>(*old_ptr);
		f(*new_ptr);
		while(!ptr.compare_exchange_strong(old_ptr, new_ptr)) {
			*new_ptr = *old_ptr;
			f(*new_ptr);
		};
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_COPY_ON_WRITE_HPP_ */
