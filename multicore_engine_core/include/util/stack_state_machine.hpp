/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/stack_state_machine.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_STACK_STATE_MACHINE_HPP_
#define UTIL_STACK_STATE_MACHINE_HPP_

#include <memory>
#include <type_traits>
#include <vector>

namespace mce {
namespace util {

template <typename Base_State, typename State_Machine>
struct stack_state_machine_default_policy;

template <template <typename...> class policy_template, typename Base_State>
struct stack_state_machine_policy_binder {
	template <typename State_Machine>
	using policy = policy_template<Base_State, State_Machine>;
};

template <typename Base_State, typename Context_Type = void,
		  template <typename> class State_Policy = stack_state_machine_policy_binder<
				  stack_state_machine_default_policy, Base_State>::template policy,
		  template <typename...> class Stack_Container = std::vector>
class stack_state_machine;

template <typename Base_State, typename State_Machine>
struct stack_state_machine_default_policy {
	using owning_ptr_t = std::unique_ptr<Base_State>;
	using ptr_t = Base_State*;
	static constexpr ptr_t ptr_t_initial = nullptr;
	ptr_t get_ptr(const owning_ptr_t& owning_ptr) {
		return owning_ptr.get();
	}
	template <typename T, typename... Args>
	owning_ptr_t enter_state(State_Machine& state_machine, const ptr_t& parent_state, Args&&... args) {
		return std::make_unique<T>(state_machine, parent_state, std::forward<Args>(args)...);
	}
	template <typename... Args>
	void leave_state_push(const ptr_t& state) {
		state->leave_push();
	}
	void leave_state_pop(const ptr_t& state) {
		state->leave_pop();
	}
	template <typename... Args>
	void reenter_state(const ptr_t& state, Args&&... args) {
		state->reenter(std::forward<Args>(args)...);
	}
};

namespace detail {

template <typename Context_Type, typename = void>
class stack_state_machine_context_wrapper {
	Context_Type context_;

public:
	typedef Context_Type context_type;
	explicit stack_state_machine_context_wrapper(const Context_Type& context) : context_(context) {}
	const Context_Type& context() const {
		return context_;
	}
	Context_Type& context() {
		return context_;
	}
};

template <typename Context_Type>
class stack_state_machine_context_wrapper<Context_Type,
										  std::enable_if_t<std::is_reference<Context_Type>::value>> {
	Context_Type context_;

public:
	typedef Context_Type context_type;
	explicit stack_state_machine_context_wrapper(const Context_Type& context) : context_(context) {}
	Context_Type context() const {
		return context_;
	}
};

template <>
class stack_state_machine_context_wrapper<void> {
public:
	typedef void context_type;
	stack_state_machine_context_wrapper() {}
};

} // namespace detail

template <typename Base_State, typename Context_Type, template <typename> class State_Policy,
		  template <typename...> class Stack_Container>
class stack_state_machine : public detail::stack_state_machine_context_wrapper<Context_Type> {
	using state_policy = State_Policy<stack_state_machine>;
	state_policy policy;
	Stack_Container<typename state_policy::owning_ptr_t> state_stack_;
	typename state_policy::ptr_t current_state_ = state_policy::ptr_t_initial;

public:
	using detail::stack_state_machine_context_wrapper<Context_Type>::stack_state_machine_context_wrapper;

	template <typename State, typename... Args>
	void enter_state(Args&&... args) {
		if(current_state_) policy.leave_state_push(current_state_);
		state_stack_.push_back(policy.template enter_state<State>(*this, current_state_, std::forward<Args>(args)...));
		current_state_ = policy.get_ptr(state_stack_.back());
	}

	template <typename... Args>
	bool pop_state(Args&&... args) {
		if(state_stack_.size() <= 1) return false;
		policy.leave_state_pop(current_state_);
		state_stack_.pop_back();
		auto state = policy.get_ptr(state_stack_.back());
		policy.reenter_state(state,std::forward<Args>(args)...);
		current_state_ = state;
		return true;
	}

	typename state_policy::ptr_t current_state() {
		return current_state_;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_STACK_STATE_MACHINE_HPP_ */
