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

template <typename Base_State, typename Context_Type, typename State_Machine>
struct stack_state_machine_default_policy;

template <typename Base_State, typename Context_Type>
struct stack_state_machine_default_policy_binder {
	template <typename State_Machine>
	using policy = stack_state_machine_default_policy<Base_State, Context_Type, State_Machine>;
};

template <typename Base_State, typename Context_Type,
		  template <typename...> class Stack_Container = std::vector,
		  template <typename> class State_Policy =
				  stack_state_machine_default_policy_binder<Base_State, Context_Type>::template policy>
class stack_state_machine;

template <typename Base_State, typename Context_Type, typename State_Machine>
struct stack_state_machine_default_policy {
	using owning_ptr_t = std::unique_ptr<Base_State>;
	using ptr_t = Base_State*;
	static constexpr ptr_t ptr_t_initial = nullptr;
	ptr_t get_ptr(const owning_ptr_t& owning_ptr) {
		return owning_ptr.get();
	}
	template <typename T, typename... Args>
	owning_ptr_t enter_state(State_Machine& state_machine, std::add_lvalue_reference_t<Context_Type> context,
							 const ptr_t& parent_state, Args&&... args) {
		return std::make_unique<T>(state_machine, context, parent_state, std::forward<Args>(args)...);
	}
	template <typename... Args>
	void leave_state_push(const ptr_t& state) {
		state->leave_push();
	}
	void leave_state_pop(const ptr_t& state) {
		state->leave_pop();
	}
	void reenter_state(const ptr_t& state) {
		state->reenter();
	}
};

template <typename Base_State, typename Context_Type, template <typename...> class Stack_Container,
		  template <typename> class State_Policy>
class stack_state_machine {
	using state_policy = State_Policy<stack_state_machine>;
	state_policy policy;
	Stack_Container<typename state_policy::owning_ptr_t> state_stack_;
	typename state_policy::ptr_t current_state_ = state_policy::ptr_t_initial;
	Context_Type context_;

public:
	stack_state_machine(Context_Type context) : context_(context) {}

	template <typename State, typename... Args>
	void enter_state(Args&&... args) {
		if(current_state_) policy.leave_state_push(current_state_);
		state_stack_.push_back(
				policy.enter_state<State>(*this, context_, current_state_, std::forward<Args>(args)...));
		current_state_ = policy.get_ptr(state_stack_.back());
	}

	template <typename... Args>
	bool pop_state(Args&&... args) {
		if(state_stack_.size() <= 1) return false;
		policy.leave_state_pop(current_state_);
		state_stack_.pop_back();
		auto state = policy.get_ptr(state_stack_.back());
		policy.reenter_state(state);
		current_state_ = state;
		return true;
	}

	std::add_lvalue_reference_t<Context_Type> context() {
		return context_;
	}

	typename state_policy::ptr_t current_state() {
		return current_state_;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_STACK_STATE_MACHINE_HPP_ */
