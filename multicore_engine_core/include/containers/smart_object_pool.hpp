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
#include "scratch_pad_pool.hpp"
#include "smart_pool_ptr.hpp"
#include "../memory/aligned_new.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

namespace mce {
namespace containers {

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

	typedef detail::smart_object_pool_block_interface::ref_count_t ref_count_t;
	typedef unsigned int ref_count_tag_t;

	struct tagged_ref_count {
		alignas(sizeof(ref_count_t)+sizeof(ref_count_tag_t))
		ref_count_t count;
		ref_count_tag_t version;
	};

	static tagged_ref_count pre_increment_tagged_ref_count(std::atomic<tagged_ref_count>& rc) {
		tagged_ref_count tmp = rc.load();
		tagged_ref_count res;
		while(!rc.compare_exchange_weak(tmp, res = {tmp.count + 1, tmp.version + 1}))
			;
		return res;
	}

	static tagged_ref_count pre_decrement_tagged_ref_count(std::atomic<tagged_ref_count>& rc) {
		tagged_ref_count tmp = rc.load();
		tagged_ref_count res;
		while(!rc.compare_exchange_weak(tmp, res = {tmp.count - 1, tmp.version + 1}))
			;
		return res;
	}

	static void store_tagged_ref_count(std::atomic<tagged_ref_count>& rc, ref_count_t new_ref_count) {
		tagged_ref_count tmp = rc.load();
		while(!rc.compare_exchange_weak(tmp, {new_ref_count, tmp.version + 1}))
			;
	}

	struct ref_count_ {
		std::atomic<tagged_ref_count> strong;
		std::atomic<ref_count_t> weak;
	};

	struct block : public detail::smart_object_pool_block_interface {
		ALIGNED_NEW_AND_DELETE(block)
		block_entry entries[block_size];
		alignas(cacheline_alignment) ref_count_ ref_counts[block_size];
		alignas(cacheline_alignment) smart_object_pool<T, block_size>* owning_pool;
		std::atomic<block*> next_block{nullptr};
		std::atomic<block*> prev_block;
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
			  block* prev_block = nullptr) noexcept : owning_pool{owning_pool},
													  prev_block{prev_block} {
			ref_counts[block_size - 1].strong = {-1, 0u};
			ref_counts[block_size - 1].weak = 0;
			entries[block_size - 1].next_free = prev;
			for(auto i = block_size - 1; i > size_t(0); i--) {
				auto index = i - 1;
				entries[index].next_free = {&entries[i], this};
				ref_counts[index].strong = {-1, 0u};
				ref_counts[index].weak = 0;
			}
			prev = {&entries[0], this};
			if(prev_block) prev_block->next_block = this;
		}

		// Only releases the blocks resources, it is the pools responsibility to ensure that the block doesn't
		// contain any objects that are alive or have weak refs on them at this point.
		~block() noexcept = default;

		virtual ref_count_t strong_ref_count(void* object) noexcept override {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			return rc.strong.load().count;
		}
		virtual void increment_strong_ref(void* object) noexcept override {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			pre_increment_tagged_ref_count(rc.strong);
		}
		virtual void increment_weak_ref(void* object) noexcept override {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			rc.weak++;
		}
		virtual void decrement_strong_ref(void* object) noexcept override {
			auto* entry = reinterpret_cast<block_entry*>(object);
			auto& rc = ref_count(entry);
			auto rc_result = pre_decrement_tagged_ref_count(rc.strong);
			if(rc_result.count == 0) {
				if(owning_pool->active_iterators > 0) {
					owning_pool->mark_for_deferred_destruction(entry, this, rc_result.version);
				} else {
					if(try_to_destroy_object(entry, rc_result.version)) {
						// Object itself carries a weakref
						decrement_weak_ref(&entry->object);
					}
				}
			}
		}
		virtual void decrement_weak_ref(void* object) noexcept override {
			auto* entry = reinterpret_cast<block_entry*>(object);
			auto& rc = ref_count(entry);
			if(--(rc.weak) == 0) { owning_pool->deallocate(entry, this); }
		}
		// Needs to be called for incrementing the strong ref count if it isn't sure that the object is alive.
		virtual bool upgrade_ref(void* object) noexcept override {
			auto& rc = ref_count(reinterpret_cast<block_entry*>(object));
			auto old = rc.strong.load();
			do { // Try to increment strong ref count while checking if the object is alive
				if(old.count < 0) return false; // Object is already dead -> fail
			} while(!rc.strong.compare_exchange_weak(old, {old.count + 1, old.version + 1}));
			return true;
		}
		bool try_to_destroy_object(block_entry* entry, ref_count_tag_t version_tag) noexcept {
			auto& rc = ref_count(entry);
			// Attempt to set ref_count to -1 as a flag marking it as dead.
			tagged_ref_count expected_ref_count = {0, version_tag};
			if(!rc.strong.compare_exchange_strong(expected_ref_count, {-1, version_tag + 1})) {
				return false; // Something got a strong ref through upgrade
			}
			--active_objects;
			--(owning_pool->active_objects);
			entry->object.~T();
			return true;
		}
		// Creates object with 1 strong ref to it
		template <typename... Args>
		void create_object(block_entry* place, Args&&... args) {
			auto& rc = ref_count(place);
			assert(rc.strong.load().count == -1); // Assert there is no object living in this place
			assert(rc.weak == 0);				  // Assert nothing is observing the place
			rc.weak = 1;						  // Object itself gets a weak ref to hold its entry
			new(&place->object) T(std::forward<Args>(args)...);
			++active_objects;
			++(owning_pool->active_objects);
			store_tagged_ref_count(rc.strong, 1);
		}
	};

	struct pending_destruction_list_entry {
		block_entry_link ptr;
		ref_count_tag_t version;
	};

	ALIGNED_NEW_AND_DELETE(smart_object_pool)

	std::atomic<size_t> active_iterators{0};

	alignas(cacheline_alignment) std::atomic<size_t> active_objects{0};

	alignas(cacheline_alignment) std::mutex free_list_mutex;
	block_entry_link first_free_entry = {nullptr, nullptr};
	std::vector<std::unique_ptr<block>> blocks;
	std::atomic<size_t> allocated_objects{0};
	std::atomic<block*> first_block{nullptr};
	std::atomic<size_t> block_count{0};

	alignas(cacheline_alignment) std::atomic<size_t> pending_destructions{0};
	std::mutex pending_destruction_list_mutex;
	std::vector<pending_destruction_list_entry> pending_destruction_list;

	static scratch_pad_pool<std::vector<pending_destruction_list_entry>> pending_destruction_scratch_pads;

	friend class smart_pool_ptr<T>;

	block_entry_link allocate() {
		std::lock_guard<std::mutex> lock(free_list_mutex);
		if(!first_free_entry.entry) { grow(); }
		auto free_entry = first_free_entry;
		first_free_entry = free_entry.entry->next_free;
		++allocated_objects;
		++(free_entry.containing_block->allocated_objects);
		return free_entry;
	}

	// May only be called when holding a lock on free list mutex
	void grow() {
		if(blocks.empty()) {
			blocks.emplace_back(std::make_unique<block>(this, first_free_entry));
			first_block = blocks.front().get();
		} else { blocks.emplace_back(std::make_unique<block>(this, first_free_entry, blocks.back().get())); }
		++block_count;
	}

	void deallocate(block_entry* entry, block* block) noexcept {
		std::lock_guard<std::mutex> lock(free_list_mutex);
		deallocate_inner(entry, block);
	}

	// May only be called when already holding lock on free list
	void deallocate_inner(block_entry* entry, block* block) noexcept {
		entry->next_free = first_free_entry;
		first_free_entry = {entry, block};
		--(block->allocated_objects);
		--allocated_objects;
	}

	void mark_for_deferred_destruction(block_entry* entry, block* block, ref_count_tag_t version_tag)
	/*TODO Find a way to make this noexcept because it will be called in a destructor*/ {
		std::lock_guard<std::mutex> lock(pending_destruction_list_mutex);
		if(active_iterators > 0) { // Check again because iterator might already be finished
			pending_destruction_list.push_back(
					pending_destruction_list_entry{block_entry_link{entry, block}, version_tag});
			++pending_destructions;
		} else {
			if(block->try_to_destroy_object(entry, version_tag)) {
				// Object itself carries a weakref
				block->decrement_weak_ref(&entry->object);
			}
		}
	}

	// Call this only after decrementing the iterator counter
	void process_deferred_destruction()
	/*TODO Find a way to make this noexcept because it will be called in a destructor*/ {
		if(pending_destructions == 0) return;
		auto pending = pending_destruction_scratch_pads.get();
		auto dealloc = pending_destruction_scratch_pads.get();
		{
			std::lock_guard<std::mutex> lock(pending_destruction_list_mutex);
			std::swap(*pending, pending_destruction_list);
		}
		for(auto& obj : *pending) {
			if(obj.ptr.containing_block->try_to_destroy_object(obj.ptr.entry, obj.version)) {
				auto& rc = obj.ptr.containing_block->ref_count(obj.ptr.entry);
				if(--(rc.weak) == 0) { dealloc->push_back(obj); }
			}
		}
		std::lock_guard<std::mutex> lock(free_list_mutex);
		for(auto& obj : *dealloc) { deallocate_inner(obj.ptr.entry, obj.ptr.containing_block); }
	}

public:
	smart_object_pool() noexcept {}
	~smart_object_pool() noexcept {
		if(allocated_objects > 0) {
			std::cerr << "Attempt to destroy smart_object_pool which has alive objects in it. "
						 "Continuing would leave dangling pointers. Calling std::terminate now." << std::endl;
			std::terminate();
		}
	}
	smart_object_pool(const smart_object_pool&) = delete;
	smart_object_pool& operator=(const smart_object_pool&) = delete;
	smart_object_pool(smart_object_pool&&) noexcept = delete;
	smart_object_pool& operator=(smart_object_pool&&) noexcept = delete;

	template <typename It_T, typename Target_T>
	class iterator_ : public std::iterator<std::forward_iterator_tag, It_T> {
		Target_T target;
		smart_object_pool<T, block_size>* pool;
		friend class smart_object_pool<T, block_size>;

		iterator_(Target_T target, smart_object_pool<T, block_size>* pool) : target(target), pool{pool} {
			++(pool->active_iterators);
			skip_until_valid();
		}

		void drop_iterator() {
			if(pool) {
				if(--(pool->active_iterators) == 0) { pool->process_deferred_destruction(); }
			}
		}

	public:
		iterator_() : target{nullptr, nullptr}, pool{nullptr} {}
		~iterator_() {
			drop_iterator();
		}

		iterator_(const iterator_<T, block_entry_link>& it) noexcept
				: target{it.target.entry, it.target.containing_block},
				  pool{it.pool} {
			if(pool) ++(pool->active_iterators);
		}

		iterator_& operator=(const iterator_<T, block_entry_link>& it) noexcept {
			target = {it.target.entry, it.target.containing_block};
			if(pool != it.pool) {
				drop_iterator();
				pool = it.pool;
				if(pool) ++(pool->active_iterators);
			}
			return *this;
		}

		iterator_(iterator_<T, block_entry_link>&& it) noexcept
				: target{it.target.entry, it.target.containing_block},
				  pool{it.pool} {
			it.target.entry = nullptr;
			it.target.containing_block = nullptr;
			it.pool = nullptr;
		}

		iterator_& operator=(iterator_<T, block_entry_link>&& it) noexcept {
			target = {it.target.entry, it.target.containing_block};
			if(pool != it.pool) {
				drop_iterator();
				pool = it.pool;
			}
			it.target.entry = nullptr;
			it.target.containing_block = nullptr;
			it.pool = nullptr;
			return *this;
		}

		typename iterator_<It_T, Target_T>::reference operator*() const {
			assert(target.containing_block);
			assert(target.entry);
			return target.entry->object;
		}
		typename iterator_<It_T, Target_T>::pointer operator->() const {
			assert(target.containing_block);
			assert(target.entry);
			return &(target.entry->object);
		}
		bool operator==(const iterator_<const T, const_block_entry_link>& it) const {
			return it.target.entry == target.entry && it.target.containing_block == target.containing_block;
		}
		bool operator!=(const iterator_<const T, const_block_entry_link>& it) const {
			return !(*this == it);
		}
		bool operator==(const iterator_<T, block_entry_link>& it) const {
			return it.target.entry == target.entry && it.target.containing_block == target.containing_block;
		}
		bool operator!=(const iterator_<T, block_entry_link>& it) const {
			return !(*this == it);
		}

		iterator_& operator++() {
			target.entry++;
			skip_until_valid();
			return *this;
		}
		iterator_ operator++(int) {
			auto it = *this;
			this->operator++();
			return it;
		}

		operator smart_pool_ptr<It_T>() const {
			assert(target.containing_block);
			assert(target.entry);
			if(target.containing_block->upgrade_ref(target.element)) {
				return smart_pool_ptr<It_T>(target.element, target.containing_block);
			} else { return smart_pool_ptr<It_T>(); }
		}

	private:
		void skip_empty_blocks() {
			// Skip over empty blocks without looking at individual entries
			while(target.containing_block) {
				if(target.containing_block->active_objects)
					break;
				else
					target.containing_block = target.containing_block->next_block;
			}
			target.entry = target.containing_block ? target.containing_block->entries : nullptr;
		}
		void skip_until_valid() {
			if(!target.containing_block) {
				target.entry = nullptr;
				return;
			} else if(target.containing_block->active_objects == 0) { skip_empty_blocks(); }
			for(;;) {
				if(!target.containing_block) {
					target.entry = nullptr;
					return;
				}
				if(target.entry >= target.containing_block->entries + block_size) {
					target.containing_block = target.containing_block->next_block;
					if(target.containing_block) { skip_empty_blocks(); }
					if(!target.containing_block) {
						target.entry = nullptr;
						return;
					}
				}
				if(!target.containing_block) {
					target.entry = nullptr;
					return;
				}
				if(target.containing_block->ref_count(target.entry).strong.load().count > 0)
					return;
				else
					target.entry++;
			}
		}
	};

	typedef iterator_<T, block_entry_link> iterator;
	typedef iterator_<const T, const_block_entry_link> const_iterator;

	template <typename... Args>
	smart_pool_ptr<T> emplace(Args&&... args) {
		auto entry = allocate();
		entry.containing_block->create_object(entry.entry, std::forward<Args>(args)...);
		return smart_pool_ptr<T>(&entry.entry->object, entry.containing_block);
	}

	bool empty() const {
		return active_objects == 0;
	}
	size_t size() const {
		return active_objects;
	}
	size_t capacity() const {
		return block_count * block_size;
	}

	void reserve(size_t reserved_size) {
		if(capacity() < reserved_size) {
			std::lock_guard<std::mutex> lock(free_list_mutex);
			while(capacity() < reserved_size) { grow(); }
		}
	}

	iterator begin() {
		block* block = first_block;
		if(block)
			return iterator({block->entries, block}, this);
		else
			return iterator();
	}

	const_iterator begin() const {
		block* block = first_block;
		if(block)
			return const_iterator({block->entries, block}, this);
		else
			return const_iterator();
	}

	const_iterator cbegin() const {
		block* block = first_block;
		if(block)
			return const_iterator({block->entries, block}, this);
		else
			return const_iterator();
	}

	iterator end() {
		return iterator();
	}

	const_iterator end() const {
		return const_iterator();
	}

	const_iterator cend() const {
		return const_iterator();
	}
};

template <typename T, size_t block_size>
scratch_pad_pool<std::vector<typename smart_object_pool<T, block_size>::pending_destruction_list_entry>>
		smart_object_pool<T, block_size>::pending_destruction_scratch_pads;

} // namespace containers
} // namespace mce

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* CONTAINERS_SMART_OBJECT_POOL_HPP_ */
