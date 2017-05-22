/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/actor.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_ACTOR_HPP_
#define UTIL_ACTOR_HPP_

/**
 * \file
 * Defines a generic actor implementation.
 */

#include <cstdint>
#include <tuple>
#include <type_traits>
#include <mce/util/local_function.hpp>
#include <mce/util/message_queue.hpp>
#include <utility>

namespace mce {
namespace util {

/// Provides a simple actor interface for objects that allows deferred operations on it from other threads.
/**
 * The actor interface provides a thread-save queue of operations on the target object of type T represented
 * trough local_function objects with the maximum functor buffer size of Max_Size.
 *
 * The operations can be \link enqueue enqueued\endlink from different threads.
 * They are executed using #process from a single thread.
 */
template <typename T, size_t Max_Size>
class actor {
private:
	template <typename R, typename... Types>
	struct member_functor {
		typedef R (T::*member_function_t)(Types...);
		member_function_t member_function;
		std::tuple<Types...> values;
		template <typename... Types2>
		member_functor(member_function_t member_function, Types2&&... values)
				: member_function(member_function), values(std::forward<Types2>(values)...) {}
		template <size_t... index>
		void call_helper(T* target, std::index_sequence<index...>) {
			(target->*member_function)(std::get<index>(values)...);
		}
		void operator()(T* target) {
			call_helper(target, std::make_index_sequence<sizeof...(Types)>());
		}
	};

public:
	/// Creates an actor interface for the given target object.
	explicit actor(T* target) : target(target) {}

	/// Enqueues an operation on the target object using a function object with the signature void(T*).
	template <typename F, typename R = decltype(std::declval<std::decay_t<F>>()(std::declval<T*>())),
			  typename Dummy = std::enable_if<!std::is_member_function_pointer<std::decay<F>>::value>>
	void enqueue(F&& f) {
		actions.push(f);
	}
	/// Enqueues a call to the given member function of the target object with the given arguments.
	/**
	 * The given argument values are saved in the functor in the queue until the call is processed.
	 */
	template <typename R, typename... Args, typename... Args2>
	void enqueue(R (T::*member_function)(Args...), Args2&&... args) {
		actions.push(member_functor<R, Args...>(member_function, std::forward<Args2>(args)...));
	}

	/// Processes the queued operations.
	void process() {
		local_function<Max_Size, void(T*)> action;
		while(actions.try_pop(action)) {
			action(target);
		}
	}

private:
	T* target;
	message_queue<local_function<Max_Size, void(T*)>> actions;
};

} // namespace util
} // namespace mce

#endif /* UTIL_ACTOR_HPP_ */
