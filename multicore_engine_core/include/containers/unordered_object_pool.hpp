/*
 * Multi-Core Engine project
 * File /mutlicore_engine_core/include/containers/unordered_object_pool.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_UNORDERED_OBJECT_POOL_HPP_
#define CONTAINERS_UNORDERED_OBJECT_POOL_HPP_

#include <memory>
#include <cassert>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <iterator>
#include <mutex>
#include <atomic>

namespace mce {
namespace containers {

namespace unordered_object_pool_lock_policies {

// Different threads can work on different objects in the same pool object. But regular data race rules apply
// for each object.
struct safe_internals_policy {
	template <typename T>
	using sync_type = std::atomic<T>;
	typedef std::mutex lock;
	typedef std::lock_guard<std::mutex> lock_guard;
	typedef std::unique_lock<std::mutex> lock_guard_delayed;
	static constexpr bool safe = true;
};

// Use no synchronization for internal management structures. Mutable pool object is not thread-safe at all.
struct unsafe_internals_policy {
	template <typename T>
	using sync_type = T;
	struct lock_ {
		void lock() {}
		void unlock() {}
	};
	typedef lock_ lock;
	struct lock_guard {
		lock_guard(lock&) {}
	};
	typedef lock_guard lock_guard_delayed;
	typedef void unsafe;
	static constexpr bool safe = false;
};

} // namespace unordered_object_pool_lock_policies

template <typename T, size_t block_size = 0x10000u,
		  typename Lock_Policy = unordered_object_pool_lock_policies::safe_internals_policy>
class unordered_object_pool {
private:
	union block_entry;
	struct block;
	template <typename U>
	using sync_type = typename Lock_Policy::template sync_type<U>;
	typedef typename Lock_Policy::lock lock;
	typedef typename Lock_Policy::lock_guard lock_guard;
	typedef typename Lock_Policy::lock_guard_delayed lock_guard_delayed;

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

	struct block {
		block_entry entries[block_size];
		sync_type<bool> active_flags[block_size];
		sync_type<size_t> active_objects{0};
		sync_type<block*> next_block{nullptr};
		sync_type<block*> prev_block;

		block(const block& other)
				: active_objects(other.active_objects), next_block{nullptr}, prev_block{nullptr} {
			for(size_t i = 0; i < block_size; ++i) {
				active_flags[i] = other.active_flags[i];
				if(active_flags[i]) {
					entries[i].object = other.entries[i].object;
				} else {
					entries[i].next_free = {nullptr, nullptr};
				}
			}
		}
		block& operator=(const block& other) {
			active_objects = other.active_objects;
			for(size_t i = 0; i < block_size; ++i) {
				active_flags[i] = other.active_flags[i];
				if(active_flags[i]) {
					entries[i].object = other.entries[i].object;
				} else {
					entries[i].next_free = {nullptr, nullptr};
				}
			}
			return *this;
		}
		block(block&&) = delete;
		block& operator=(block&&) = delete;

		sync_type<bool>& active(const block_entry* entry) noexcept {
			assert(entries <= entry && entry <= (entries + block_size - 1));
			size_t index = entry - entries;
			return *(active_flags + index);
		}

		bool active(const block_entry* entry) const noexcept {
			assert(entries <= entry && entry <= (entries + block_size - 1));
			size_t index = entry - entries;
			return *(active_flags + index);
		}

		bool contains(const block_entry* entry) const {
			return entries <= entry && entry <= (entries + block_size - 1);
		}

		block(block_entry_link& prev, block* prev_block = nullptr) noexcept : prev_block{prev_block} {
			active_flags[block_size - 1] = false;
			entries[block_size - 1].next_free = prev;
			for(auto i = block_size - 1; i > size_t(0); i--) {
				auto index = i - 1;
				entries[index].next_free = {&entries[i], this};
				active_flags[index] = false;
			}
			prev = {&entries[0], this};
			if(prev_block) prev_block->next_block = this;
		}

		~block() noexcept {
			block* next_blk = this->next_block;
			block* prev_blk = this->prev_block;
			if(next_blk) next_blk->prev_block = prev_blk;
			if(prev_blk) prev_blk->next_block = next_blk;
			for(size_t i = 0; i < block_size; ++i) {
				if(active_flags[i]) entries[i].object.~T();
			}
		}

		void clear(block_entry* entry) noexcept {
			auto& a = active(entry);
			if(a) {
				--active_objects;
				a = false;
				entry->object.~T();
				entry->next_free = {nullptr, nullptr};
			}
		}

		void fill(block_entry* entry, const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value) {
			clear(entry);
			new(&entry->object) T(value);
			active(entry) = true;
			++active_objects;
		}

		void fill(block_entry* entry, T&& value) noexcept(std::is_nothrow_move_constructible<T>::value) {
			clear(entry);
			new(&entry->object) T(std::move(value));
			active(entry) = true;
			++active_objects;
		}

		template <typename... Args>
		void emplace(block_entry* entry,
					 Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
			clear(entry);
			new(&entry->object) T(std::forward<Args>(args)...);
			active(entry) = true;
			++active_objects;
		}
	};

	lock management_data_lock;
	block_entry_link first_free_entry = {nullptr, nullptr};
	std::vector<std::unique_ptr<block>> blocks;
	sync_type<size_t> active_objects{0};
	sync_type<block*> first_block{nullptr};
	sync_type<size_t> block_count{0};

public:
	unordered_object_pool() noexcept {}
	~unordered_object_pool() noexcept = default;
	unordered_object_pool(const unordered_object_pool& other)
			: first_free_entry{nullptr, nullptr}, active_objects{0} {
		lock_guard_delayed ul0(management_data_lock);
		lock_guard_delayed ul1(other.management_data_lock);
		if(Lock_Policy::safe) std::lock(ul0, ul1);

		active_objects = other.active_objects;
		blocks.reserve(other.blocks.size());
		std::transform(other.blocks.begin(), other.blocks.end(), std::back_inserter(blocks),
					   [](auto& b) { return std::make_unique<block>(*b); });
		fix_block_ptrs();
		size_t free_entries = recalculate_freelist();
		assert(active_objects + free_entries == capacity());
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
		(void)free_entries;
	}
	unordered_object_pool(unordered_object_pool&& other) noexcept : first_free_entry{nullptr, nullptr},
																	active_objects{0} {
		lock_guard_delayed ul0(management_data_lock);
		lock_guard_delayed ul1(other.management_data_lock);
		if(Lock_Policy::safe) std::lock(ul0, ul1);

		first_free_entry = other.first_free_entry;
		blocks = std::move(other.blocks);
		active_objects = other.active_objects;
		other.first_free_entry = {nullptr, nullptr};
		other.active_objects = 0;
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
	}
	unordered_object_pool& operator=(const unordered_object_pool& other) {
		if(this == &other) return *this;
		lock_guard_delayed ul0(management_data_lock);
		lock_guard_delayed ul1(other.management_data_lock);
		if(Lock_Policy::safe) std::lock(ul0, ul1);

		active_objects = other.active_objects;
		blocks.clear();
		first_free_entry = {nullptr, nullptr};
		blocks.reserve(other.blocks.size());
		std::transform(other.blocks.begin(), other.blocks.end(), std::back_inserter(blocks),
					   [](auto& b) { return std::make_unique<block>(*b); });
		fix_block_ptrs();
		size_t free_entries = recalculate_freelist();
		assert(active_objects + free_entries == capacity());
		(void)free_entries;
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
		return *this;
	}
	unordered_object_pool& operator=(unordered_object_pool&& other) noexcept {
		lock_guard_delayed ul0(management_data_lock);
		lock_guard_delayed ul1(other.management_data_lock);
		if(Lock_Policy::safe) std::lock(ul0, ul1);

		first_free_entry = other.first_free_entry;
		blocks = std::move(other.blocks);
		active_objects = other.active_objects;
		other.first_free_entry = {nullptr, nullptr};
		other.active_objects = 0;
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
		return *this;
	}

	template <typename It_T, typename Target_T>
	class iterator_ : public std::iterator<std::forward_iterator_tag, It_T> {
		Target_T target;
		friend class unordered_object_pool<T, block_size>;

		iterator_(Target_T target) : target(target) {
			skip_until_valid();
		}

	public:
		iterator_() : target{nullptr, nullptr} {}

		iterator_(const iterator_<T, block_entry_link>& it) noexcept
				: target{it.target.entry, it.target.containing_block} {}

		iterator_& operator=(const iterator_<T, block_entry_link>& it) noexcept {
			target = {it.target.entry, it.target.containing_block};
			return *this;
		}

		typename iterator_<It_T, Target_T>::reference operator*() const {
			assert(target.containing_block);
			assert(target.containing_block->active(target.entry));
			return target.entry->object;
		}
		typename iterator_<It_T, Target_T>::pointer operator->() const {
			assert(target.containing_block);
			assert(target.containing_block->active(target.entry));
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

		operator It_T*() const {
			assert(target.containing_block);
			assert(target.containing_block->active(target.entry));
			return &(target.entry->object);
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
			} else if(target.containing_block->active_objects == 0) {
				skip_empty_blocks();
			}
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
				if(target.containing_block->active(target.entry))
					return;
				else
					target.entry++;
			}
		}
	};

	typedef iterator_<T, block_entry_link> iterator;
	typedef iterator_<const T, const_block_entry_link> const_iterator;

	iterator insert(const T& value) {
		block_entry_link value_entry = {nullptr, nullptr};
		{
			lock_guard lg(management_data_lock);
			if(!first_free_entry.entry) grow();
			value_entry = first_free_entry;
			first_free_entry = value_entry.entry->next_free;
		}
		value_entry.containing_block->fill(value_entry.entry, value);
		++active_objects;
		return iterator({value_entry.entry, value_entry.containing_block});
	}

	iterator insert(T&& value) {
		block_entry_link value_entry = {nullptr, nullptr};
		{
			lock_guard lg(management_data_lock);
			if(!first_free_entry.entry) grow();
			value_entry = first_free_entry;
			first_free_entry = value_entry.entry->next_free;
		}
		value_entry.containing_block->fill(value_entry.entry, std::move(value));
		++active_objects;
		return iterator({value_entry.entry, value_entry.containing_block});
	}

	template <typename... Args>
	iterator emplace(Args&&... args) {
		block_entry_link value_entry = {nullptr, nullptr};
		{
			lock_guard lg(management_data_lock);
			if(!first_free_entry.entry) grow();
			value_entry = first_free_entry;
			first_free_entry = value_entry.entry->next_free;
		}
		value_entry.containing_block->emplace(value_entry.entry, std::forward<Args>(args)...);
		++active_objects;
		return iterator({value_entry.entry, value_entry.containing_block});
	}

	iterator begin() {
		block* start_block = first_block;
		if(start_block)
			return iterator({start_block->entries, start_block});
		else
			return iterator();
	}

	const_iterator begin() const {
		block* start_block = first_block;
		if(start_block)
			return const_iterator({start_block->entries, start_block});
		else
			return const_iterator();
	}

	const_iterator cbegin() const {
		block* start_block = first_block;
		if(start_block)
			return const_iterator({start_block->entries, start_block});
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

	iterator erase(iterator pos) {
		assert(pos.target.containing_block);
		assert(pos.target.containing_block->active(pos.target.entry));
		--active_objects;
		pos.target.containing_block->clear(pos.target.entry);
		{
			lock_guard lg(management_data_lock);
			pos.target.entry->next_free = first_free_entry;
			first_free_entry = pos.target;
		}
		pos.skip_until_valid();
		return pos;
	}

	iterator erase(iterator first, iterator last) {
		while(first != last) first = erase(first);
		return first;
	}

	const_iterator find(const T& object) const {
		auto obj_entry = reinterpret_cast<const block_entry*>(&object);
		for(block* b = first_block; b; b = b->next_block) {
			if(b->contains(obj_entry)) return const_iterator({obj_entry, b});
		}
		return const_iterator();
	}

	iterator find(T& object) {
		auto obj_entry = reinterpret_cast<block_entry*>(&object);
		for(block* b = first_block; b; b = b->next_block) {
			if(b->contains(obj_entry)) return iterator({obj_entry, b});
		}
		return iterator();
	}

	void find_and_erase(T& object) {
		auto it = find(object);
		if(it != end()) erase(it);
	}

	void clear_and_reorganize() {
		lock_guard lg(management_data_lock);
		blocks.clear();
		block_count = 0;
		first_block = nullptr;
		active_objects = 0;
		first_free_entry = {nullptr, nullptr};
	}

	void clear() {
		lock_guard lg(management_data_lock);
		first_free_entry = {nullptr, nullptr};
		block_entry_link* free = &first_free_entry;
		for(auto& b : blocks) {
			for(size_t i = 0; i < block_size; ++i) {
				block_entry* entry_ptr = b->entries + i;
				if(b->active_flags[i]) {
					b->active_flags[i] = false;
					entry_ptr->object.~T();
				}
				*free = {entry_ptr, b.get()};
				free = &(entry_ptr->next_free);
			}
			b->active_objects = 0;
		}
		*free = {nullptr, nullptr};
		active_objects = 0;
	}

	size_t reorganize() {
		if(active_objects == capacity()) return 0;
		if(!active_objects) {
			clear_and_reorganize();
			return 0;
		}
		lock_guard lg(management_data_lock);
		block_entry_link it = {blocks.front()->entries, blocks.front().get()};
		block_entry_link it2 = {blocks.back()->entries + block_size - 1, blocks.back().get()};

		size_t reallocated_objects = 0;
		while(it != it2) {
			for(; it != it2; ++it) {
				if(!it.containing_block->active(it.entry)) break;
			}
			for(; it2 != it; --it2) {
				if(it2.containing_block->active(it2.entry)) break;
			}
			if(it != it2) {
				it.containing_block->fill(it.entry, std::move_if_noexcept(it2.entry->object));
				it2.containing_block->clear(it2.entry);
				++reallocated_objects;
			}
		}
		if(reallocated_objects) {
			blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [](auto& b) {
							 return b->active_objects == 0;
						 }), blocks.end());
			block_count = blocks.size();
			if(blocks.empty()) {
				first_block = nullptr;
			} else {
				first_block = blocks.front().get();
			}
			size_t free_entries = recalculate_freelist_last_block();
			assert(active_objects + free_entries == capacity());
			(void)free_entries;
		}
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
		return reallocated_objects;
	}

	size_t size() const {
		return active_objects;
	}

	size_t capacity() const {
		return block_count * block_size;
	}

	bool empty() const {
		return active_objects == 0;
	}

private:
	// May only be called when holding lock
	void grow() {
		if(blocks.empty()) {
			blocks.emplace_back(std::make_unique<block>(first_free_entry));
		} else {
			blocks.emplace_back(std::make_unique<block>(first_free_entry, blocks.back().get()));
		}
		block_count = blocks.size();
		if(blocks.empty()) {
			first_block = nullptr;
		} else {
			first_block = blocks.front().get();
		}
	}
	// May only be called when holding lock
	size_t recalculate_freelist_last_block() {
		size_t free_entries = 0;
		block_entry_link* free = &first_free_entry;
		if(!blocks.empty()) {
			for(size_t i = 0; i < block_size; ++i) {
				block_entry* entry_ptr = blocks.back()->entries + i;
				if(!blocks.back()->active_flags[i]) {
					*free = {entry_ptr, blocks.back().get()};
					free = &(entry_ptr->next_free);
					++free_entries;
				}
			}
		}
		*free = {nullptr, nullptr};
		return free_entries;
	}
	// May only be called when holding lock
	size_t recalculate_freelist() {
		size_t free_entries = 0;
		block_entry_link* free = &first_free_entry;
		for(auto& b : blocks) {
			for(size_t i = 0; i < block_size; ++i) {
				block_entry* entry_ptr = b->entries + i;
				if(!b->active_flags[i]) {
					*free = {entry_ptr, b.get()};
					free = &(entry_ptr->next_free);
					++free_entries;
				}
			}
		}
		*free = {nullptr, nullptr};
		return free_entries;
	}
	// May only be called when holding lock
	void fix_block_ptrs() {
		if(blocks.empty()) return;
		blocks.front()->prev_block = nullptr;
		for(size_t i = 0; i < blocks.size(); ++i) {
			if(i > 0) blocks[i]->prev_block = blocks[i - 1].get();
			if(i < blocks.size() - 1) blocks[i]->next_block = blocks[i + 1].get();
		}
		blocks.back()->next_block = nullptr;
	}
};
}
// namespace containers
} // namespace mce

#endif /* CONTAINERS_UNORDERED_OBJECT_POOL_HPP_ */
