/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/scratch_pad_pool.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SCRATCH_PAD_POOL_HPP_
#define CONTAINERS_SCRATCH_PAD_POOL_HPP_

#include <mutex>
#include <deque>
#include <stack>
#include <memory>

namespace mce {
namespace containers {

// Can only be used efficiently for movable types.
template <typename T>
class scratch_pad_pool {
private:
	std::mutex pool_mutex;
	std::stack<T> pool;
	void give_back(T&& obj){
		std::lock_guard<std::mutex> lock(pool_mutex);
		pool.push(std::move(obj));
	}
public:
	class object {
		scratch_pad_pool<T>* pool;
		T obj;
	public:
		object(scratch_pad_pool<T>* pool, T&& obj) : pool(pool), obj(std::move(obj)) {}
		object(const object&) = delete;
		object& operator=(const object&) = delete;
		object(object&&) noexcept = default;
		object& operator=(object&&) noexcept = default;
		~object() noexcept {
			pool->give_back(std::move(obj));
		}
		T& operator*() {
			return obj;
		}
		const T& operator*() const{
			return obj;
		}
		T* operator->() {
			return &obj;
		}
		const T* operator->() const {
			return &obj;
		}
	};
	object get(){
		std::lock_guard<std::mutex> lock(pool_mutex);
		if(pool.empty()){
			return object(this,T());
		}
		else{
			object o(this,std::move(pool.top()));
			pool.pop();
			return o;
		}
	}
};

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SCRATCH_PAD_POOL_HPP_ */
