/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/smart_pool_ptr.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SMART_POOL_PTR_HPP_
#define CONTAINERS_SMART_POOL_PTR_HPP_

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
	typedef long long ref_count_t;
	virtual ref_count_t strong_ref_count(void* object) noexcept = 0;
	virtual void increment_strong_ref(void* object) noexcept = 0;
	virtual void increment_weak_ref(void* object) noexcept = 0;
	virtual void decrement_strong_ref(void* object) noexcept = 0;
	virtual void decrement_weak_ref(void* object) noexcept = 0;
	virtual bool upgrade_ref(void* object) noexcept = 0;
	virtual ~smart_object_pool_block_interface() noexcept = default;
};
}

// Interface follows that of std::shared_ptr as closely as possible.
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

	smart_pool_ptr(T* object, detail::smart_object_pool_block_interface* block) noexcept
			: object{object},
			  managed_object{object},
			  block{block} {}
	smart_pool_ptr(T* object, void* managed_object, detail::smart_object_pool_block_interface* block) noexcept
			: object{object},
			  managed_object{managed_object},
			  block{block} {}

public:
	typedef detail::smart_object_pool_block_interface::ref_count_t ref_count_t;
	smart_pool_ptr() noexcept : object{nullptr}, managed_object{nullptr}, block{nullptr} {}
	smart_pool_ptr(const smart_pool_ptr& other) noexcept : object{other.object},
														   managed_object{other.managed_object},
														   block{other.block} {
		if(block) block->increment_strong_ref(managed_object);
	}
	smart_pool_ptr(smart_pool_ptr&& other) noexcept : object{other.object},
													  managed_object{other.managed_object},
													  block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
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
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
	smart_pool_ptr(const smart_pool_ptr<U>& other) noexcept : object(other.object),
															  managed_object{other.managed_object},
															  block{other.block} {
		if(block) block->increment_strong_ref(managed_object);
	}
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
	smart_pool_ptr(smart_pool_ptr<U>&& other) noexcept : object(other.object),
														 managed_object{other.managed_object},
														 block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	template <typename U>
	explicit smart_pool_ptr(weak_pool_ptr<U>&& other)
			: object(other.object), managed_object{other.managed_object}, block{other.block} {
		if(!block) { throw std::bad_weak_ptr(); }
		if(!block->upgrade_ref(managed_object)) { throw std::bad_weak_ptr(); }
	}
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
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
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
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
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
	~smart_pool_ptr() {
		if(block) block->decrement_strong_ref(managed_object);
	}

	void swap(smart_pool_ptr& other) noexcept {
		using std::swap;
		swap(object, other.object);
		swap(managed_object, other.managed_object);
		swap(block, other.block);
	}

	void reset(){
		if(block) block->decrement_strong_ref(managed_object);
		object=nullptr;
		managed_object=nullptr;
		block=nullptr;
	}

	T* get() const noexcept {
		return object;
	}

	T& operator*() const noexcept {
		assert(block);
		assert(object);
		return *object;
	}

	T* operator->() const noexcept {
		assert(block);
		assert(object);
		return object;
	}

	ref_count_t use_count() const noexcept {
		if(!block) return 0;
		if(!object) return 0;
		auto rc = block->strong_ref_count(managed_object);
		assert(rc > 0);
		return rc;
	}

	bool unique() const noexcept {
		return use_count() == 1;
	}

	explicit operator bool() const noexcept {
		if(!block) return false;
		if(object)
			return true;
		else
			return false;
	}
};

// Interface follows that of std::weak_ptr as closely as possible.
template <typename T>
class weak_pool_ptr {
	T* object;
	void* managed_object; // May differ from object if this weak_pool_ptr was constructed using the aliasing
						  // constructor
	detail::smart_object_pool_block_interface* block;

public:
	template <typename U>
	friend class smart_pool_ptr;

	typedef detail::smart_object_pool_block_interface::ref_count_t ref_count_t;
	weak_pool_ptr() noexcept : object{nullptr}, managed_object{nullptr}, block{nullptr} {}
	weak_pool_ptr(const weak_pool_ptr& other) noexcept : object{other.object},
														 managed_object{other.managed_object},
														 block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
	weak_pool_ptr(weak_pool_ptr&& other) noexcept : object{other.object},
													managed_object{other.managed_object},
													block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
	weak_pool_ptr(const weak_pool_ptr<U>& other) noexcept : object(other.object),
															managed_object{other.managed_object},
															block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
	weak_pool_ptr(weak_pool_ptr<U>&& other) noexcept : object(other.object),
													   managed_object{other.managed_object},
													   block{other.block} {
		other.object = nullptr;
		other.managed_object = nullptr;
		other.block = nullptr;
	}
	template <typename U>
	weak_pool_ptr(smart_pool_ptr<U>&& other)
			: object(other.object), managed_object{other.managed_object}, block{other.block} {
		if(block) block->increment_weak_ref(managed_object);
	}
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
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
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
	template <typename U>
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
	// TODO: Verify is this satisfies the requirement that also applies to the equivalent in std::shared_ptr
	// to only participate in overload resolution if U* is convertible to T*.
	template <typename U>
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
	~weak_pool_ptr() {
		if(block) block->decrement_weak_ref(managed_object);
	}

	void reset(){
		if(block) block->decrement_weak_ref(managed_object);
		object=nullptr;
		managed_object=nullptr;
		block=nullptr;
	}

	void swap(weak_pool_ptr& other) noexcept {
		using std::swap;
		swap(object, other.object);
		swap(managed_object, other.managed_object);
		swap(block, other.block);
	}

	ref_count_t use_count() const noexcept {
		if(!block) return 0;
		if(!managed_object) return 0;
		if(!object) return 0;
		return block->strong_ref_count(managed_object);
	}

	bool expired() const noexcept {
		if(!block) return true;
		if(!managed_object) return true;
		if(!object) return true;
		return use_count() < 0;
	}

	smart_pool_ptr<T> lock() const noexcept {
		if(!object || !managed_object || !block) return smart_pool_ptr<T>();
		if(block->upgrade_ref(managed_object))
			return smart_pool_ptr<T>(object, managed_object, block);
		else
			return smart_pool_ptr<T>();
	}
};

template<typename T>
void swap(smart_pool_ptr<T>& a, smart_pool_ptr<T>& b){
	a.swap(b);
}
template<typename T>
void swap(weak_pool_ptr<T>& a, weak_pool_ptr<T>& b){
	a.swap(b);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SMART_POOL_PTR_HPP_ */
