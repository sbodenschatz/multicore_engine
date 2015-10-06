/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/smart_object_pool.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SMART_OBJECT_POOL_HPP_
#define CONTAINERS_SMART_OBJECT_POOL_HPP_

#include <memory>
#include <cassert>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <iterator>
#include <atomic>
#include <iostream>
#include <mutex>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

namespace mce {
namespace containers {

template <typename T>
class smart_pool_ptr;

namespace detail {
// Interface used for type erasure
template <typename T>
struct smart_object_pool_block_interface {
	virtual void increment_strong_ref(T* object) noexcept = 0;
	virtual void increment_weak_ref(T* object) noexcept = 0;
	virtual void decrement_strong_ref(T* object) noexcept = 0;
	virtual void decrement_weak_ref(T* object) noexcept = 0;
	virtual bool upgrade_ref(T* object) noexcept = 0;
	virtual ~smart_object_pool_block_interface() noexcept = default;
};
}

template <typename T, size_t block_size = 0x10000u>
class smart_object_pool {
private:
	static const int cacheline_alignment = 64;

	union block_entry;
	struct block;

	struct block_entry_link {
		block_entry* entry;
		block* containing_block;
		block_entry_link& operator++() {
			if(entry && containing_block) {
				if(entry == containing_block->entries + block_size - 1) {
					containing_block = containing_block->next_block;
					if(containing_block)
						entry = containing_block->entries;
					else
						entry = nullptr;
				} else
					++entry;
			}
			return *this;
		}
		block_entry_link& operator--() {
			if(entry && containing_block) {
				if(entry == containing_block->entries) {
					containing_block = containing_block->prev_block;
					if(containing_block)
						entry = containing_block->entries + block_size - 1;
					else
						entry = nullptr;
				} else
					--entry;
			}
			return *this;
		}
		bool operator==(const block_entry_link& other) const {
			return other.entry == entry && other.containing_block == containing_block;
		}
		bool operator!=(const block_entry_link& other) const {
			return !(*this == other);
		}
	};

	struct const_block_entry_link {
		const block_entry* entry;
		const block* containing_block;
		const_block_entry_link& operator++() {
			if(entry && containing_block) {
				if(entry == containing_block->entries + block_size - 1) {
					containing_block = containing_block->next_block;
					if(containing_block)
						entry = containing_block->entries;
					else
						entry = nullptr;
				} else
					++entry;
			}
			return *this;
		}
		const_block_entry_link& operator--() {
			if(entry && containing_block) {
				if(entry == containing_block->entries) {
					containing_block = containing_block->prev_block;
					if(containing_block)
						entry = containing_block->entries + block_size - 1;
					else
						entry = nullptr;
				} else
					--entry;
			}
			return *this;
		}
		bool operator==(const const_block_entry_link& other) const {
			return other.entry == entry && other.containing_block == containing_block;
		}
		bool operator!=(const const_block_entry_link& other) const {
			return !(*this == other);
		}
	};

	union block_entry {
		T object;
		block_entry_link next_free;
		block_entry() noexcept : next_free{nullptr, nullptr} {}
		~block_entry() noexcept {}
		block_entry(const block_entry&) = delete;
		block_entry& operator=(const block_entry&) = delete;
		block_entry(block_entry&&) = delete;
		block_entry& operator=(block_entry&&) = delete;
	};

	typedef long long ref_count_t;

	struct ref_count_ {
		std::atomic<ref_count_t> strong;
		std::atomic<ref_count_t> weak;
	};

	struct block : public detail::smart_object_pool_block_interface<T> {
		block_entry entries[block_size];
		alignas(cacheline_alignment) ref_count_ ref_counts[block_size];
		block* next_block = nullptr;
		block* prev_block;
		smart_object_pool<T, block_size>* owning_pool;
		alignas(cacheline_alignment) std::atomic<size_t> allocated_objects{0};
		alignas(cacheline_alignment) std::atomic<size_t> active_objects{0};

		block(const block& other) = delete;
		block& operator=(const block& other) = delete;
		block(block&&) = delete;
		block& operator=(block&&) = delete;

		ref_count_& ref_count(const block_entry* entry) noexcept {
			assert(entries <= entry && entry <= (entries + block_size - 1));
			size_t index = entry - entries;
			return *(ref_counts + index);
		}

		const ref_count_& ref_count(const block_entry* entry) const noexcept {
			assert(entries <= entry && entry <= (entries + block_size - 1));
			size_t index = entry - entries;
			return *(ref_counts + index);
		}

		bool contains(const block_entry* entry) const {
			return entries <= entry && entry <= (entries + block_size - 1);
		}

		// May only be called inside of a lock on free list
		block(smart_object_pool<T, block_size>* owning_pool, block_entry_link& prev,
			  block* prev_block = nullptr) noexcept : owning_pool(owning_pool),
													  prev_block(prev_block) {
			ref_counts[block_size - 1].strong = 0;
			ref_counts[block_size - 1].weak = 0;
			entries[block_size - 1].next_free = prev;
			for(auto i = block_size - 1; i > size_t(0); i--) {
				auto index = i - 1;
				entries[index].next_free = {&entries[i], this};
				ref_counts[index].strong = 0;
				ref_counts[index].weak = 0;
			}
			prev = {&entries[0], this};
			if(prev_block) this->prev_block->next_block = this;
		}

		// May only be called inside of a lock on free list
		~block() noexcept {
			if(next_block) next_block->prev_block = prev_block;
			if(prev_block) prev_block->next_block = next_block;
			if(allocated_objects > 0) {
				std::cerr << "Attempt to destroy smart_object_pool::block which has alive objects in it. "
							 "Continuing would leave dangling pointers. Calling std::terminate now."
						  << std::endl;
				std::terminate();
			}
		}

		virtual void increment_strong_ref(T* object) noexcept {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			rc.strong++;
		}
		virtual void increment_weak_ref(T* object) noexcept {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			rc.weak++;
		}
		virtual void decrement_strong_ref(T* object) noexcept {
			auto* entry = reinterpret_cast<block_entry*>(object);
			auto& rc = ref_count(entry);
			if(--(rc.strong) == 0) {
				if(owning_pool->active_iterators > 0) {
					owning_pool->mark_for_deferred_destruction(entry, this);
				} else {
					try_to_destroy_object(entry);
				}
			}
		}
		virtual void decrement_weak_ref(T* object) noexcept {
			auto* entry = reinterpret_cast<block_entry*>(object);
			auto& rc = ref_count(entry);
			rc.weak--;
			if(--(rc.weak) == 0) {
				owning_pool->deallocate(entry, this);
				--allocated_objects;
			}
		}
		// Needs to be called for incrementing the strong ref count if it isn't sure that the object is alive.
		virtual bool upgrade_ref(T* object) noexcept {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			ref_count_t old = rc.strong;
			do { // Try to increment strong ref count while checking if the object is alive
				if(old < 0) return false; // Object is dead -> fail
			} while(!rc.strong.compare_exchange_weak(old, old + 1));
			return true;
		}
		bool try_to_destroy_object(block_entry* entry) noexcept {
			auto& rc = ref_count(entry);
			// Attempt to set ref_count to -1 as a flag marking it as dead.
			ref_count_t expected_ref_count = 0;
			if(!rc.strong.compare_exchange_strong(expected_ref_count, -1)) {
				return false; // Something got a strong ref through upgrade
			}
			--active_objects;
			entry->object.~T();
			// Object itself carries a weakref
			decrement_weak_ref(&entry->object);
			return true;
		}
		//Creates object with 1 strong ref to it
		template <typename... Args>
		void create_object(block_entry* place, Args&&... args) {
			auto& rc = ref_count(place);
			assert(rc.strong == -1); // Assert there is no object living in this place
			assert(rc.weak == 0); // Assert nothing is observing the place
			--allocated_objects;
			rc.weak = 1; // Object itself gets a weak ref to hold its entry
			new(place->object) T(std::forward<Args>(args)...);
			++active_objects;
			rc.strong = 1;
		}
	};

	std::atomic<size_t> active_iterators{0};

	alignas(cacheline_alignment) std::atomic<size_t> active_objects{0};

	alignas(cacheline_alignment) std::mutex free_list_mutex;
	block_entry_link first_free_entry = {nullptr, nullptr};
	std::vector<std::unique_ptr<block>> blocks;
	std::atomic<size_t> allocated_objects;

	alignas(cacheline_alignment) std::mutex pending_destruction_list_mutex;
	std::vector<block_entry_link> pending_destruction_list;

	friend class smart_pool_ptr<T>;

	block_entry* allocate() {
		// TODO implement
		return nullptr;
	}

	void deallocate(block_entry* /* entry*/, block* /*b*/) noexcept {
		// TODO implement
	}

	void mark_for_deferred_destruction(block_entry* /* entry*/, block* /*b*/)
	/*TODO Find a way to make this noexcept because it will be called in a destructor*/ {
		// TODO implement
	}

	void process_deferred_destruction()
	/*TODO Find a way to make this noexcept because it will be called in a destructor*/ {
		// TODO implement
	}

public:
	smart_object_pool() noexcept {}
	~smart_object_pool() noexcept {
		if(allocated_objects > 0) {
			std::cerr << "Attempt to destroy smart_object_pool which has alive objects in it. "
						 "Continuing would leave dangling pointers. Calling std::terminate now."
					  << std::endl;
			std::terminate();
		}
	}
	smart_object_pool(const smart_object_pool&) = delete;
	smart_object_pool& operator=(const smart_object_pool&) = delete;
	smart_object_pool(smart_object_pool&&) noexcept = delete;
	smart_object_pool& operator=(smart_object_pool&&) noexcept = delete;
};

} // namespace containers
} // namespace mce

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* CONTAINERS_SMART_OBJECT_POOL_HPP_ */
