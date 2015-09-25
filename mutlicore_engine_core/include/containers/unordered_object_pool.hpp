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

template<typename T, size_t block_size = 0x10000u>
class unordered_object_pool {
private:
	union block_entry;
	struct block;

	struct block_entry_link {
		block_entry* entry;
		block* containing_block;
	};

	union block_entry {
		T object;
		typename unordered_object_pool<T, block_size>::block_entry_link next_free;
		block_entry() noexcept:next_free {nullptr,nullptr} {}
		~block_entry()noexcept {}
		block_entry(const block_entry&)=delete;
		block_entry& operator=(const block_entry&)=delete;
		block_entry(block_entry&&)=delete;
		block_entry& operator=(block_entry&&)=delete;
	};

	struct block {
		typename unordered_object_pool<T,block_size>::block_entry entries[block_size];
		bool active_flags[block_size];
		std::unique_ptr<block> next_block;
		block(const block&)=delete;
		block& operator=(const block&)=delete;
		block(block&&)=delete;
		block& operator=(block&&)=delete;

		bool& active(block_entry* entry) noexcept {
			assert(entries<=entry && entry<(entries+block_size));
			size_t index = entry-entries;
			return *(active_flags+index);
		}

		block(block_entry_link& prev) noexcept {
			active_flags[block_size-1]=false;
			entries[block_size-1].next_free = prev;
			for(auto i = block_size-1;i>size_t(0);i--) {
				auto index=i-1;
				entries[index].next_free = {&entries[i],this};
				active_flags[index]=false;
			}
			prev= {&entries[0],this};
		}

		~block()noexcept {
			for(size_t i=0;i<block_size;++i) {
				if(active_flags[i]) entries[i].object.~T();
			}
		}

		void clear(block_entry* entry) noexcept {
			auto& a = active(entry);
			if(a) {
				entry->object.~T();
				a=false;
				entry->next_free= {nullptr,nullptr};
			}
		}

		void fill(block_entry* entry,const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value) {
			clear(entry);
			new (&entry->object) T(value);
			active(entry)=true;
		}

		void fill(block_entry* entry,T&& value) noexcept(std::is_nothrow_move_constructible<T>::value) {
			clear(entry);
			new (&entry->object) T(std::move(value));
			active(entry)=true;
		}

		template<typename... Args>
		void emplace(block_entry* entry,Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value) {
			clear(entry);
			new (&entry->object) T(std::forward<Args>(args)...);
			active(entry)=true;
		}
	};

	block_entry_link first_free_entry= {nullptr,nullptr};
	std::unique_ptr<block> first_block;

public:
	unordered_object_pool() noexcept {}

	template<typename It_T>
	class iterator_:public std::iterator<std::forward_iterator_tag,It_T> {
		block_entry_link target;
		friend class unordered_object_pool<T,block_size>;

	public:
		iterator_():target {nullptr,nullptr} {}
		iterator_(const block_entry_link& target):target(target) {
			skip_until_valid();
		}

		iterator_(const iterator_<typename std::remove_cv<It_T>::type>& it):target(it.target){}
		iterator_& operator=(const iterator_<typename std::remove_cv<It_T>::type>& it){
			target=it.target;
			return *this;
		}

		typename iterator_<T>::reference operator*() const {
			assert(target.containing_block);
			assert(target.containing_block->active(target.entry));
			return target.entry->object;
		}
		typename iterator_<T>::pointer operator->() const {
			assert(target.containing_block);
			assert(target.containing_block->active(target.entry));
			return &(target.entry->object);
		}
		bool operator==(const iterator_& it) const {
			return it.target.entry==target.entry && it.target.containing_block==target.containing_block;
		}
		bool operator!=(const iterator_& it) const {
			return !(*this==it);
		}

		iterator_& operator++() {
			target.entry++;
			skip_until_valid();
			return *this;
		}
		iterator_ operator++(int) {
			auto it=*this;
			this->operator++();
			return it;
		}

	private:
		void skip_until_valid() {
			if(!target.containing_block) {
				target.entry=nullptr;
				return;
			}
			while(true) {
				if(target.entry>=target.containing_block->entries+block_size) {
					target.containing_block=target.containing_block->next_block.get();
					if(target.containing_block) {
						target.entry=target.containing_block->entries;
					}
					else {
						target.entry=nullptr;
						return;
					}
				}
				if(target.containing_block->active(target.entry)) return;
				else target.entry++;
			}
		}
	};

	typedef iterator_<T> iterator;
	typedef iterator_<const T> const_iterator;

	void insert(const T& value) {
		if(!first_free_entry.entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry.entry->next_free;
		value_entry.containing_block->fill(value_entry.entry,value);
		first_free_entry=next_free;
	}

	void insert(T&& value) {
		if(!first_free_entry.entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry.entry->next_free;
		value_entry.containing_block->fill(value_entry.entry,std::move(value));
		first_free_entry=next_free;
	}

	template<typename... Args>
	void emplace(Args&&... args) {
		if(!first_free_entry.entry) grow();
		auto value_entry=first_free_entry;
		auto next_free = value_entry.entry->next_free;
		value_entry.containing_block->emplace(value_entry.entry,std::forward<Args>(args)...);
		first_free_entry=next_free;
	}

	iterator begin() {
		if(first_block) return iterator( {first_block->entries,first_block.get()});
		else return iterator();
	}

	const_iterator begin() const {
		if(first_block) return const_iterator( {first_block->entries,first_block.get()});
		else return const_iterator();
	}

	const_iterator cbegin() const {
		if(first_block) return const_iterator( {first_block->entries,first_block.get()});
		else return const_iterator();
	}

	iterator end(){
		return iterator();
	}

	const_iterator end() const{
		return const_iterator();
	}

	const_iterator cend() const{
		return const_iterator();
	}

	iterator erase(iterator pos){
		assert(pos.target.containing_block);
		assert(pos.target.containing_block->active(pos.target.entry));
		pos.target.containing_block->clear(pos.target.entry);
		pos.target.entry->next_free=first_free_entry;
		first_free_entry=pos.target;
		pos.skip_until_valid();
		return pos;
	}
	iterator erase(const_iterator pos){
		assert(pos.target.containing_block);
		assert(pos.target.containing_block->active(pos.target.entry));
		pos.target.containing_block->clear(pos.target.entry);
		pos.target.entry->next_free=first_free_entry;
		first_free_entry=pos.target;
		pos.skip_until_valid();
		return iterator(pos.target);
	}

	iterator erase(iterator first, iterator last){
		while(first!=last) first=erase(first);
		return first;
	}

	iterator erase(const_iterator first, const_iterator last){
		while(first!=last) first=erase(first);
		return iterator(first.target);
	}

	void clear_and_reorganize(){
		first_block = nullptr;
		first_free_entry={nullptr,nullptr};
	}

private:
	void grow() {
		auto i = &first_block;
		while(*i) {
			i=&((*i)->next_block);
		}
		*i=std::make_unique<block>(first_free_entry);
	}
};

}
 // namespace containers
}// namespace mce

#endif /* CONTAINERS_UNORDERED_OBJECT_POOL_HPP_ */
