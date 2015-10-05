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

template <typename T, size_t block_size = 0x10000u>
class smart_object_pool {
private:
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

	struct ref_count_ {
		std::atomic<size_t> strong;
		std::atomic<size_t> weak;
	};

	struct block {
		block_entry entries[block_size];
		ref_count_ ref_counts[block_size];
		std::atomic<size_t> allocated_objects{0};
		block* next_block = nullptr;
		block* prev_block;

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
		block(block_entry_link& prev, block* prev_block = nullptr) noexcept : prev_block(prev_block) {
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
	};

	static const int cacheline_alignment = 64;

	std::atomic<size_t> active_iterators{0};

	alignas(cacheline_alignment) std::atomic<size_t> active_objects{0};

	alignas(cacheline_alignment) std::mutex free_list_mutex;
	block_entry_link first_free_entry = {nullptr, nullptr};
	std::vector<std::unique_ptr<block>> blocks;
	std::atomic<size_t> allocated_objects;

	alignas(cacheline_alignment) std::mutex pending_destruction_list_mutex;
	std::vector<block_entry_link> pending_destruction_list;

public:
	smart_object_pool() noexcept {}
	~smart_object_pool() noexcept = default;
};

} // namespace containers
} // namespace mce

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* CONTAINERS_SMART_OBJECT_POOL_HPP_ */
