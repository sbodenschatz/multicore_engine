/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/smart_pool_ptr.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SMART_POOL_PTR_HPP_
#define CONTAINERS_SMART_POOL_PTR_HPP_

/**
 * \file
 * Defines the smart pointers used for smart_object_pool.
 */

#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace mce {
namespace containers {

template <typename T>
class weak_pool_ptr;

template <typename U, size_t block_size>
class smart_object_pool;

namespace detail {
// Interface used for type erasure to remove template parameters of the pool
// from the type of smart_pool_ptr instances.
struct smart_object_pool_block_interface {
	typedef int ref_count_t;
	virtual ref_count_t strong_ref_count(void* object) noexcept = 0;
	virtual void increment_strong_ref(void* object) noexcept = 0;
	virtual void increment_weak_ref(void* object) noexcept = 0;
	virtual void decrement_strong_ref(void* object) noexcept = 0;
	virtual void decrement_weak_ref(void* object) noexcept = 0;
	virtual bool upgrade_ref(void* object) noexcept = 0;
	virtual ~smart_object_pool_block_interface() noexcept = default;
};
}

/// Smart pointer class that manages the lifetime of objects in a smart_object_pool.
/**
 * The interface of the class follows that of std::shared_ptr as closely as feasible.
 */
template <typename T>
class smart_pool_ptr {
	T* object;
	void* managed_object; // May differ from object if this smart_pool_ptr was constructed using the aliasing
						  // constructor
	detail::smart_object_pool_block_interface* block;

	template <typename U, size_t block_size>
	friend class smart_object_pool;
	template <typename U>
	friend class weak_pool_ptr;
	template <typename U>
	friend class smart_pool_ptr;

	smart_pool_ptr(T* object, detail::smart_object_pool_block_interface* block) noexcept
			: object{object},
			  managed_object{object},
			  block{block} {}
	smart_pool_ptr(T* object, void* managed_object, detail::smart_object_pool_block_interface* block) noexcept
			: object{object},
			  managed_object{managed_object},
			  block{block} {}

	typedef detail::smart_object_pool_block_interface::ref_count_t ref_count_t;

public:
	/// Creates and empty smart_pool_ptr, meaning one that does not manage an object.
	smart_pool_ptr() noexcept : object{nullptr}, managed_object{nullptr}, block{nullptr} {}
	/// Allows copy-construction of smart_pool_ptr.
	smart_pool_ptr(const smart_pool_ptr& other) noexcept : object{other.object},
														   managed_object{other.managed_object},
														   block{other.block} {
		if(block) block->increment_strong_ref(managed_object);
	}
	/// Allows move-construction of smart_pool_ptr.
	smart_pool_ptr(smart_pool_ptr&& other) noexcept : object{other.object},
													  managed_object{other.managed_object},
													  block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	/// Creates an aliasing smart_pool_ptr.
	/**
	 * This constructor creates a smart_pool_ptr that shares ownership with other but uses ptr for the
	 * accessor members (get, operator*, operator->).
	 * Theoretically ptr can be an unrelated pointer, as long as it stays valid at least as long as the object
	 * whose ownership is shared by the new pointer an other.
	 * However, in many useful cases ptr and the object managed by other are in some kind of relationship. A
	 * typical example for the use of this constructor is handing out pointers to sub-objects of the object
	 * managed by other that should participate in the shared ownership.
	 */
	template <typename U>
	smart_pool_ptr(const smart_pool_ptr<U>& other, T* ptr) noexcept : object{ptr},
																	  managed_object{other.managed_object},
																	  block{other.block} {
		if(!block || !object || !managed_object) {
			block = nullptr;
			object = nullptr;
			managed_object = nullptr;
		}
		if(block) block->increment_strong_ref(managed_object);
	}
	/// Allows copy-constructing from a pointer-assignment-compatible other smart_pool_ptr template instance.
	/**
	 * The smart_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	smart_pool_ptr(const smart_pool_ptr<U>& other) noexcept : object(other.object),
															  managed_object{other.managed_object},
															  block{other.block} {
		if(block) block->increment_strong_ref(managed_object);
	}
	/// Allows move-constructing from a pointer-assignment-compatible other smart_pool_ptr template instance.
	/**
	 * The smart_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	smart_pool_ptr(smart_pool_ptr<U>&& other) noexcept : object(other.object),
														 managed_object{other.managed_object},
														 block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	/// Allows to construct a smart_pool_ptr by moving from a pointer-assignment-compatible weak_pool_ptr.
	/**
	 * The smart_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	// TODO Check, if this signature is correct or should be const L-ref.
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	explicit smart_pool_ptr(weak_pool_ptr<U>&& other)
			: object(other.object), managed_object{other.managed_object}, block{other.block} {
		if(!block) {
			throw std::bad_weak_ptr();
		}
		if(!block->upgrade_ref(managed_object)) {
			throw std::bad_weak_ptr();
		}
	}
	/// Allows copy-assignment of smart_pool_ptr.
	smart_pool_ptr<T>& operator=(const smart_pool_ptr& other) noexcept {
		if(other.managed_object == managed_object) {
			object = other.object;
			return *this;
		}
		if(block) block->decrement_strong_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		if(block) block->increment_strong_ref(managed_object);
		return *this;
	}
	/// Allows copy-assignment from a pointer-assignment-compatible other smart_pool_ptr template instance.
	/**
	 * The smart_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be assigned from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	smart_pool_ptr<T>& operator=(const smart_pool_ptr<U>& other) noexcept {
		if(other.managed_object == managed_object) {
			object = other.object;
			return *this;
		}
		if(block) block->decrement_strong_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		if(block) block->increment_strong_ref(managed_object);
		return *this;
	}
	/// Allows move-assignment for smart_pool_ptr.
	smart_pool_ptr<T>& operator=(smart_pool_ptr&& other) noexcept {
		assert(this != &other);
		if(block) block->decrement_strong_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
		return *this;
	}
	/// Allows move-assignment from a pointer-assignment-compatible other smart_pool_ptr template instance.
	/**
	 * The smart_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be assigned from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	smart_pool_ptr<T>& operator=(smart_pool_ptr<U>&& other) noexcept {
		assert(this != &other);
		if(block) block->decrement_strong_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
		return *this;
	}

	/// \brief  If the smart_pool_ptr takes part in an object's ownership, one reference to the object is
	/// dropped and if it was the last reference the object is either deleted immediately or marked for
	/// deletion.
	~smart_pool_ptr() {
		if(block) block->decrement_strong_ref(managed_object);
	}

	/// Swaps the values of *this and other.
	void swap(smart_pool_ptr& other) noexcept {
		using std::swap;
		swap(object, other.object);
		swap(managed_object, other.managed_object);
		swap(block, other.block);
	}

	/// \brief  If the smart_pool_ptr takes part in an object's ownership, one reference to the object is
	/// dropped and if it was the last reference the object is either deleted immediately or marked for
	/// deletion. The smart_pool_ptr is left empty after the reset.
	void reset() {
		if(block) block->decrement_strong_ref(managed_object);
		object = nullptr;
		managed_object = nullptr;
		block = nullptr;
	}

	/// Returns a pointer to the object referenced by the smart_pool_ptr.
	T* get() const noexcept {
		return object;
	}

	/// Dereferences the object referenced by the smart_pool_ptr and provides access to it.
	T& operator*() const noexcept {
		assert(block);
		assert(object);
		return *object;
	}

	/// Provides access to the object referenced by the smart_pool_ptr.
	T* operator->() const noexcept {
		assert(block);
		assert(object);
		return object;
	}

	/// Returns the number of smart_pool_ptr objects that participate in the ownership of the managed object.
	ref_count_t use_count() const noexcept {
		if(!block) return 0;
		if(!object) return 0;
		auto rc = block->strong_ref_count(managed_object);
		assert(rc > 0);
		return rc;
	}

	/// Returns true if and only if this smart_pool_ptr is the only one managing the ownership of the managed
	/// object.
	bool unique() const noexcept {
		return use_count() == 1;
	}

	/// Converts the smart_pool_ptr to a bool representing the result of a not-null check.
	explicit operator bool() const noexcept {
		if(!block) return false;
		if(object)
			return true;
		else
			return false;
	}
};

/// Smart pointer class that holds a weak reference on an object in a smart_object_pool.
/**
 * A weak_pool_ptr doesn't keep the referenced object alive and doesn't allow access to the referenced object.
 * However it allows checking if the object is alive and can also be upgraded to a smart_pool_ptr when it is.
 * The upgraded smart_pool_ptr keeps the object alive while it is used and can be used to access the object.
 *
 * Interface follows that of std::weak_ptr as closely as possible.
 */
template <typename T>
class weak_pool_ptr {
	T* object;
	void* managed_object; // May differ from object if this weak_pool_ptr was constructed using the aliasing
						  // constructor
	detail::smart_object_pool_block_interface* block;

	template <typename U>
	friend class smart_pool_ptr;

	typedef detail::smart_object_pool_block_interface::ref_count_t ref_count_t;

public:
	/// Creates and empty smart_pool_ptr, meaning one that does not manage an object.
	weak_pool_ptr() noexcept : object{nullptr}, managed_object{nullptr}, block{nullptr} {}
	/// Allows copy-construction of weak_pool_ptr.
	weak_pool_ptr(const weak_pool_ptr& other) noexcept : object{other.object},
														 managed_object{other.managed_object},
														 block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
	/// Allows move-construction of weak_pool_ptr.
	weak_pool_ptr(weak_pool_ptr&& other) noexcept : object{other.object},
													managed_object{other.managed_object},
													block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	/// Allows copy-constructing from a pointer-assignment-compatible other weak_pool_ptr template instance.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * weak_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	weak_pool_ptr(const weak_pool_ptr<U>& other) noexcept : object(other.object),
															managed_object{other.managed_object},
															block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
	/// Allows move-constructing from a pointer-assignment-compatible other weak_pool_ptr template instance.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * weak_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	weak_pool_ptr(weak_pool_ptr<U>&& other) noexcept : object(other.object),
													   managed_object{other.managed_object},
													   block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	/// \brief Constructs a weak_pool_ptr referencing the same object as the given
	/// pointer-assignment-compatible smart_pool_ptr.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be constructed from)
	 * weak_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	// cppcheck-suppress noExplicitConstructor
	weak_pool_ptr(smart_pool_ptr<U>& other)
			: object(other.object), managed_object{other.managed_object}, block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
	/// Allows copy-assignment of weak_pool_ptr.
	weak_pool_ptr<T>& operator=(const weak_pool_ptr& other) noexcept {
		if(other.managed_object == managed_object) {
			object = other.object;
			return *this;
		}
		if(block) block->decrement_weak_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		if(block) block->increment_weak_ref(managed_object);
		return *this;
	}
	/// Allows copy-assignment from a pointer-assignment-compatible other weak_pool_ptr template instance.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be assigned from)
	 * weak_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	weak_pool_ptr<T>& operator=(const weak_pool_ptr<U>& other) noexcept {
		if(other.managed_object == managed_object) {
			object = other.object;
			return *this;
		}
		if(block) block->decrement_weak_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		if(block) block->increment_weak_ref(managed_object);
		return *this;
	}
	/// Allows copy-assignment from a pointer-assignment-compatible smart_pool_ptr template instance.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be assigned from)
	 * smart_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	weak_pool_ptr<T>& operator=(const smart_pool_ptr<U>& other) noexcept {
		if(other.managed_object == managed_object) {
			object = other.object;
			return *this;
		}
		if(block) block->decrement_weak_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		if(block) block->increment_weak_ref(managed_object);
		return *this;
	}
	/// Allows move-assignment of weak_pool_ptr.
	weak_pool_ptr<T>& operator=(weak_pool_ptr&& other) noexcept {
		assert(this != &other);
		if(block) block->decrement_weak_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
		return *this;
	}
	/// Allows move-assignment from a pointer-assignment-compatible other weak_pool_ptr template instance.
	/**
	 * The weak_pool_ptr\<A\> is pointer-assignment-compatible with (i.e. can be assigned from)
	 * weak_pool_ptr\<B\> if B* is implicitly convertible to A*.
	 */
	template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U*, T*>::value>>
	weak_pool_ptr<T>& operator=(weak_pool_ptr<U>&& other) noexcept {
		assert(this != &other);
		if(block) block->decrement_weak_ref(managed_object);
		object = other.object;
		managed_object = other.managed_object;
		block = other.block;
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
		return *this;
	}
	/// \brief If this weak_pool_ptr references an object (i.e. it is not empty) the weak reference is
	/// released, when all weak references are released the slot can be reused for new objects.
	~weak_pool_ptr() {
		if(block) block->decrement_weak_ref(managed_object);
	}

	/// \brief If this weak_pool_ptr references an object (i.e. it is not empty) the weak reference is
	/// released, when all weak references are released the slot can be reused for new objects. The
	/// weak_pool_ptr is left empty after the reset.
	void reset() {
		if(block) block->decrement_weak_ref(managed_object);
		object = nullptr;
		managed_object = nullptr;
		block = nullptr;
	}

	/// Swaps the values of *this and other.
	void swap(weak_pool_ptr& other) noexcept {
		using std::swap;
		swap(object, other.object);
		swap(managed_object, other.managed_object);
		swap(block, other.block);
	}

	/// Returns the number of smart_pool_ptr objects that participate in the ownership of the managed object.
	ref_count_t use_count() const noexcept {
		if(!block) return 0;
		if(!managed_object) return 0;
		if(!object) return 0;
		return block->strong_ref_count(managed_object);
	}

	/// Returns true if the referenced object was already deleted.
	bool expired() const noexcept {
		if(!block) return true;
		if(!managed_object) return true;
		if(!object) return true;
		return use_count() < 0;
	}

	/// \brief Attempts to upgrade the weak_pool_ptr to a smart_pool_ptr and returns a smart_pool_ptr to the
	/// object if the object is alive or an empty smart_pool_ptr if it is not.
	smart_pool_ptr<T> lock() const noexcept {
		if(!object || !managed_object || !block) return smart_pool_ptr<T>();
		if(block->upgrade_ref(managed_object))
			return smart_pool_ptr<T>(object, managed_object, block);
		else
			return smart_pool_ptr<T>();
	}
};

/// \brief Casts a smart_pool_ptr to another smart_pool_ptr with different object type as raw pointers would
/// be casted by static_cast.
template <typename T, typename U>
smart_pool_ptr<T> static_pointer_cast(const smart_pool_ptr<U>& orig) noexcept {
	auto p = static_cast<T*>(orig.get());
	return smart_pool_ptr<T>(orig, p);
}

/// \brief Casts a smart_pool_ptr to another smart_pool_ptr with different object type as raw pointers would
/// be casted by dynamic_cast.
template <typename T, typename U>
smart_pool_ptr<T> dynamic_pointer_cast(const smart_pool_ptr<U>& orig) noexcept {
	auto p = dynamic_cast<T*>(orig.get());
	if(p)
		return smart_pool_ptr<T>(orig, p);
	else
		return smart_pool_ptr<T>();
}

/// \brief Casts a smart_pool_ptr to another smart_pool_ptr with different object type as raw pointers would
/// be casted by const_cast.
template <typename T, typename U>
smart_pool_ptr<T> const_pointer_cast(const smart_pool_ptr<U>& orig) noexcept {
	auto p = const_cast<T*>(orig.get());
	return smart_pool_ptr<T>(orig, p);
}

/// Provides a non-member ADL swap for smart_pool_ptr.
template <typename T>
void swap(smart_pool_ptr<T>& a, smart_pool_ptr<T>& b) {
	a.swap(b);
}

/// Provides a non-member ADL swap for weak_pool_ptr.
template <typename T>
void swap(weak_pool_ptr<T>& a, weak_pool_ptr<T>& b) {
	a.swap(b);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SMART_POOL_PTR_HPP_ */
