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

namespace mce {
namespace containers {

template<typename T, size_t block_size = 1024u>
class unordered_object_pool {
private:
public:
	class block_entry;

	union block_entry_content {
		T object;
		block_entry* next_free;
		block_entry_content() noexcept:next_free(nullptr) {}
		~block_entry_content()noexcept {}
		block_entry_content(const block_entry_content&)=delete;
		block_entry_content& operator=(const block_entry_content&)=delete;
		block_entry_content(block_entry_content&&)=delete;
		block_entry_content& operator=(block_entry_content&&)=delete;
	};

	class block_entry {
		block_entry_content data;
		bool active=false;
	public:
		block_entry(const block_entry&)=delete;
		block_entry& operator=(const block_entry&)=delete;
		block_entry(block_entry&&)=delete;
		block_entry& operator=(block_entry&&)=delete;
		block_entry() noexcept {}
		~block_entry() noexcept {
			if(active) data.object.~T();
		}
		void fill(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value) {
			clear();
			new (&data.object) T(value);
			active=true;
		}
		void fill(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value) {
			clear();
			new (&data.object) T(std::move(value));
			active=true;
		}
		template<typename... Args>
		void emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
			clear();
			new (&data.object) T(std::forward<Args>(args)...);
			active=true;
		}
		void clear() noexcept {
			if(active) {
				data.object.~T();
				active=false;
				data.next_free=nullptr;
			}
		}
		block_entry* next_free() const noexcept {
			assert(!active);
			return data.next_free;
		}
		void next_free(block_entry* nf) noexcept {
			assert(!active);
			data.next_free=nf;
		}
	};

	struct block {
		block_entry entries[block_size];
		std::unique_ptr<block> next_block;
		block(const block&)=delete;
		block& operator=(const block&)=delete;
		block(block&&)=delete;
		block& operator=(block&&)=delete;
		block(block_entry*& prev) {
			entries[block_size-1].next_free(prev);
			for(auto i = block_size-1;i>size_t(0);i--) {
				auto index=i-1;
				entries[index].next_free(&entries[i]);
			}
			prev=&entries[0];
		}
	};

	block_entry* first_free_entry=nullptr;
	std::unique_ptr<block> first_block;

public:
	unordered_object_pool(){}

	void insert(const T& value) {
		if(!first_free_entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry->next_free();
		value_entry->fill(value);
		first_free_entry=next_free;
	}
	void insert(T&& value) {
		if(!first_free_entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry->next_free();
		value_entry->fill(std::move(value));
		first_free_entry=next_free;
	}
	template<typename... Args>
	void emplace(Args&&... args) {
		if(!first_free_entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry->next_free();
		value_entry->emplace(std::forward<Args>(args)...);
		first_free_entry=next_free;
	}

	void grow(){
		auto i = &first_block;
		while(*i){
			i=&((*i)->next_block);
		}
		*i=std::make_unique<block>(first_free_entry);
	}
};

}
 // namespace containers
}// namespace mce

#endif /* CONTAINERS_UNORDERED_OBJECT_POOL_HPP_ */
