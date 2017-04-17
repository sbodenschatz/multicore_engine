/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/stack_state_machine.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_STACK_STATE_MACHINE_HPP_
#define UTIL_STACK_STATE_MACHINE_HPP_

namespace mce {
namespace util {

template <typename Base_State, typename Context_Type>
struct default_stack_state_machine_policy {
	using owning_ptr_t = std::unique_ptr<Base_State>;
	using ptr_t = Base_State*;
	static constexpr ptr_t ptr_t_initial = nullptr;
	ptr_t get_ptr(const owning_ptr_t& owning_ptr) {
		return owning_ptr.get();
	}
	template <typename T, typename... Args>
	owning_ptr_t enter_state(stack_state_machine<Base_State>& state_machine, Context_Type context,
							 const ptr_t& parent_state, Args&&... args) {
		auto state = std::make_unique<T>(state_machine, context, parent_state);
		state->enter(std::forward<Args>(args)...);
		return state;
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

template <typename Base_State, typename Context_Type,
		  typename State_Policy = default_stack_state_machine_policy<Base_State, Context_Type>,
		  typename Stack_Container = std::vector<typename State_Policy::owning_ptr_t>>
class stack_state_machine {
	Stack_Container state_stack_;
	typename State_Policy::ptr_t current_state_ = State_Policy::ptr_t_initial;
	Context_Type context_;
	State_Policy policy;

public:
	template <typename Initial_State, typename... Enter_Args>
	stack_state_machine(Context_Type context, Enter_Args&&... enter_args)
			: context_(context) {
		enter_state<Initial_State>(std::forward<Enter_Args>(enter_args)...);
	}

	template <typename State, typename... Args>
	void enter_state(Args&&... args) {
		if(current_state_) policy.leave_state_push(current_state_);
		state_stack_.push_back(
				policy.enter_state(*this, context_, current_state_, std::forward<Args>(args)...));
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
	}

	Context_Type context() {
		return context_;
	}

	Base_State& current_state() {
		return *current_state_;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_STACK_STATE_MACHINE_HPP_ */
