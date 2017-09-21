/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/simple_smart_object_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_
#define MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_

/**
 * \file
 * Defines a simple smart-pointer-managed pool for objects with fixed memory locations.
 */

#include <algorithm>
#include <boost/operators.hpp>
#include <memory>
#include <mutex>
#include <vector>

namespace mce {
namespace containers {

/// \brief Provides a container for objects that are managed by smart pointers and should be iteratable
/// through the pool.
/**
 * The pool is thread-safe regarding the set objects for most member functions (exceptions are noted on the
 * members).
 * Access to the objects must be either externally synchronized by using code or must be internally
 * synchronized by the objects.
 * The thread-safety concept works phase-base by making the set of objects read-only during normal use.
 * Object additions or removals can be performed concurrently to iterating over the pool but are not visible
 * in this phase.
 * These changes are made available for future readers by calling process_pending. This operation must be
 * externally synchronized by the caller and must not run concurrently with any other member function of the
 * pool or with iterating over the pool.
 */
template <typename T>
class simple_smart_object_pool {
	std::vector<std::shared_ptr<T>> objects_;
	std::vector<std::shared_ptr<T>> pending_objects_;
	mutable std::mutex pending_objects_mtx_;

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

	/// Implements RandomAccessIterator for const and non-const iterators over the pool.
	template <typename Obj, typename It>
	class iterator_
			: public boost::additive<
					  iterator_<Obj, It>, std::ptrdiff_t,
					  boost::unit_steppable<iterator_<Obj, It>, boost::totally_ordered<iterator_<Obj, It>>>> {
		It it;

		friend class simple_smart_object_pool<T>;

		explicit iterator_(It it) : it{it} {}

	public:
		/// The type used to count the object slots for differences between iterators.
		using difference_type = std::ptrdiff_t;
		/// The type of the values referenced by the iterator.
		using value_type = T;
		/// The type of a pointer to a referenced value.
		using pointer = T*;
		/// The type of a reference to a referenced value.
		using reference = T&;
		/// A tag type indicating that this iterator implements the RandomAccessIterator concept.
		using iterator_category = std::random_access_iterator_tag;

		/// Dereferences the iterator to get a reference to the object it points to.
		reference operator*() const {
			return **it;
		}
		/// Dereferences the iterator to members of the object it points to.
		pointer operator->() const {
			return it->get();
		}
		/// \brief Dereferences the iterator using an offset to access the object n slots after the pointed-to
		/// object.
		reference operator[](std::ptrdiff_t n) const {
			return *(it[n]);
		}
		/// Returns the difference in object slots between this and other.
		std::ptrdiff_t operator-(const iterator_<Obj, It>& other) const {
			return it - other.it;
		}
		/// Advances the iterator by one slot.
		/**
		 * Post-increment is provided based on this by boost.operators.
		 */
		iterator_<Obj, It>& operator++() {
			++it;
			return *this;
		}
		/// Advances the iterator backwards by one slot.
		/**
		 * Post-decrement is provided based on this by boost.operators.
		 */
		iterator_<Obj, It>& operator--() {
			--it;
			return *this;
		}
		/// Advances the iterator by n slots.
		/**
		 * Symmetric operator+ overloads are provided based on this by boost.operators.
		 */
		iterator_<Obj, It>& operator+=(std::ptrdiff_t n) {
			it += n;
			return *this;
		}
		/// Advances the iterator backwards by n slots.
		/**
		 * The operator-(iterator_,std::ptrdiff_t) is provided based on this by boost.operators.
		 */
		iterator_<Obj, It>& operator-=(std::ptrdiff_t n) {
			it -= n;
			return *this;
		}
		/// Compares this and other for equality.
		/**
		 * The operator != is provided based on this by boost.operators.
		 */
		bool operator==(const iterator_<Obj, It>& other) const {
			return it == other.it;
		}
		/// Returns true if this refers to an object slot before other.
		/**
		 * The other comparison operators are provided based on this by boost.operators.
		 */
		bool operator<(const iterator_<Obj, It>& other) const {
			return it < other.it;
		}

		/// Allows converting a non-const iterator to a const iterator.
		operator iterator_<const T, typename std::vector<std::shared_ptr<T>>::const_iterator>() const {
			return iterator_<const T, typename std::vector<std::shared_ptr<T>>::const_iterator>(it);
		}
	};

	/// RandomAccessIterator
	typedef iterator_<T, typename std::vector<std::shared_ptr<T>>::iterator> iterator;
	/// Constant RandomAccessIterator
	typedef iterator_<const T, typename std::vector<std::shared_ptr<T>>::const_iterator> const_iterator;

	///  \brief Creates an object in the the pool and returns a smart pointer as the initial owner that
	///  manages the lifetime of the object.
	/**
	 * The added object becomes available to readers (through iterators) after process_pending() has been
	 * called.
	 */
	template <typename... Args>
	std::shared_ptr<T> emplace(Args&&... args) {
		auto o = std::make_shared<T>(std::forward<Args>(args)...);
		std::lock_guard<decltype(pending_objects_mtx_)> lock(pending_objects_mtx_);
		pending_objects_.push_back(o);
		return o;
	}
	/// Returns true if and only if the pool has no objects in it.
	bool empty() const {
		return objects_.empty();
	}
	/// Returns the number objects in the pool.
	size_t size() const {
		return objects_.size();
	}
	/// Returns the number of objects, the pool can currently hold without a larger handle container.
	size_t capacity() const {
		return objects_.capacity();
	}
	/// \brief Returns an iterator referring to the first object in the pool or a past-end-iterator if the
	/// pool is empty.
	iterator begin() {
		return iterator(objects_.begin());
	}
	/// \brief Returns an const_iterator referring to the first object in the pool or a past-end-iterator
	/// if the pool is empty.
	const_iterator begin() const {
		return const_iterator(objects_.begin());
	}
	/// \brief Returns an const_iterator referring to the first object in the pool or a past-end-iterator
	/// if the pool is empty.
	const_iterator cbegin() const {
		return const_iterator(objects_.cbegin());
	}
	/// Returns a past-end-iterator for this pool.
	iterator end() {
		return iterator(objects_.end());
	}
	/// Returns a constant past-end-iterator for this pool.
	const_iterator end() const {
		return const_iterator(objects_.end());
	}
	/// Returns a constant past-end-iterator for this pool.
	const_iterator cend() const {
		return const_iterator(objects_.cend());
	}

	/// \brief Makes changes to the object set available to future readers by removing orphaned objects and
	/// adding created ones.
	/**
	 * \warning Must be externally synchronized against all other operations on the pool.
	 */
	void process_pending() {
		// Obtain lock although this needs to be externally synchronized to acquire visibility of writes
		// performed under lock.
		std::lock_guard<decltype(pending_objects_mtx_)> lock(pending_objects_mtx_);
		objects_.reserve(objects_.size() + pending_objects_.size());
		std::move(pending_objects_.begin(), pending_objects_.end(), std::back_inserter(objects_));
		pending_objects_.clear();
		objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
									  [](const std::shared_ptr<T>& ptr) { return ptr.use_count() == 1; }),
					   objects_.end());
	}

	/// Reserves space for at least the given number of objects.
	/**
	 * \warning Must be externally synchronized against all other operations on the pool.
	 */
	void reserve(size_t size) {
		objects_.reserve(size);
	}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_SIMPLE_SMART_OBJECT_POOL_HPP_ */
