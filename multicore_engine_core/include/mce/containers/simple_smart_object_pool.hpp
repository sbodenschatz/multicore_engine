/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/simple_smart_object_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_
#define MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_

#include <algorithm>
#include <memory>
#include <shared_mutex>
#include <vector>

namespace mce {
namespace containers {

template <typename T>
class simple_smart_object_pool {
	std::vector<std::shared_ptr<T>> objects_;
	std::vector<std::shared_ptr<T>> pending_objects_;
	mutable std::shared_timed_mutex pending_objects_mtx_;

public:
	/// Creates an empty pool.
	simple_smart_object_pool() {}
	/// Destroys the pool.
	~simple_smart_object_pool() noexcept {}
	/// Forbids copying the pool.
	simple_smart_object_pool(const simple_smart_object_pool&) = delete;
	/// Forbids copying the pool.
	simple_smart_object_pool& operator=(const simple_smart_object_pool&) = delete;
	/// Forbids moving the pool.
	simple_smart_object_pool(simple_smart_object_pool&&) noexcept = delete;
	/// Forbids moving the pool.
	simple_smart_object_pool& operator=(simple_smart_object_pool&&) noexcept = delete;

	template <typename Obj>
	class iterator_ {};

	/// ForwardIterator
	typedef iterator_<T> iterator;
	/// Constant ForwardIterator
	typedef iterator_<const T> const_iterator;

	///  \brief Creates an object in the the pool and returns a smart pointer as the initial owner that
	///  manages the lifetime of the object.
	template <typename... Args>
	std::shared_ptr<T> emplace(Args&&... args) {
		auto o = std::make_shared<T>(std::forward<Args>(args)...);
		std::lock_guard<decltype(pending_objects_mtx_)> lock(pending_objects_mtx_);
		pending_objects_.push_back(o);
		return o;
	}
	/// Returns true if and only if the pool has no objects in it.
	bool empty() const;
	/// Returns the number objects in the pool.
	size_t size() const;
	/// Returns the number of objects, the pool can currently hold without a larger handle container.
	size_t capacity() const;
	/// \brief Returns an iterator referring to the first object in the pool or a past-end-iterator if the
	/// pool is empty.
	iterator begin();
	/// \brief Returns an const_iterator referring to the first object in the pool or a past-end-iterator
	/// if the pool is empty.
	const_iterator begin() const;
	/// \brief Returns an const_iterator referring to the first object in the pool or a past-end-iterator
	/// if the pool is empty.
	const_iterator cbegin() const;
	/// Returns a past-end-iterator for this pool.
	iterator end();
	/// Returns a constant past-end-iterator for this pool.
	const_iterator end() const;
	/// Returns a constant past-end-iterator for this pool.
	const_iterator cend() const;
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_ */
