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

/// \brief Allows binding a policy template that is templated for Base_State and State_Machine to template
/// template parameters that expect a template only parameterized on State_Machine.
template <template <typename...> class policy_template, typename Base_State>
struct stack_state_machine_policy_binder {
	/// Provides the bound template.
	template <typename State_Machine>
	using policy = policy_template<Base_State, State_Machine>;
};

template <typename Base_State, typename Context_Type = void,
		  template <typename> class State_Policy = stack_state_machine_policy_binder<
				  stack_state_machine_default_policy, Base_State>::template policy,
		  template <typename...> class Stack_Container = std::vector>
class stack_state_machine;

/// Implements the default policy used by stack_state_machine.
/**
 * This policy uses std::unique_ptr for state ownership and raw pointers for accessing the current state.
 * Push entering into a state is handled using the state constructor.
 * Leaving a state calls leave_push or leave_pop depending on if a new state is pushed on top of the current
 * one or the current state is popped from the stack.
 * When a state is reentered through pop, reenter is called on it.
 */
template <typename Base_State, typename State_Machine>
struct stack_state_machine_default_policy {
	/// Use std::unique_ptr for state ownership.
	using owning_ptr_t = std::unique_ptr<Base_State>;
	/// Use raw pointers for accessing the current state.
	using ptr_t = Base_State*;
	/// Signals an empty current state (stack is empty).
	static constexpr ptr_t ptr_t_empty = nullptr;
	/// Converts owning_ptr_t to ptr_t.
	ptr_t get_ptr(const owning_ptr_t& owning_ptr) {
		return owning_ptr.get();
	}
	/// Is called by the stack_state_machine when it needs to enter state T.
	template <typename T, typename... Args>
	owning_ptr_t enter_state(State_Machine& state_machine, const ptr_t& parent_state, Args&&... args) {
		return std::make_unique<T>(state_machine, parent_state, std::forward<Args>(args)...);
	}
	/// Is called by the stack_state_machine when it needs to leave the given state through push.
	template <typename... Args>
	void leave_state_push(const ptr_t& state) {
		state->leave_push();
	}
	/// Is called by the stack_state_machine when it needs to leave the given state through pop.
	void leave_state_pop(const ptr_t& state) {
		state->leave_pop();
	}
	/// Is called by the stack_state_machine when it needs to reenter the given state through pop.
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
	stack_state_machine_context_wrapper(const Context_Type& context) : context_(context) {}
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
	stack_state_machine_context_wrapper(Context_Type context) : context_(context) {}
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

/// Implements a generic stack state machine.
/**
 * The state machine has a stack of states with the top end of stack being the current state.
 * When it enters a new state (through enter_state) the new state is pushed on top of the stack.
 * The machine can return to the previous states (using pop_state) by removing the top element of the stack.
 *
 * The behavior of the stack_state_machine can be configured by providing a policy template in State_Policy,
 * for details see
 * the default policy defined in stack_state_machine_default_policy.
 *
 * The container used as stack representation can be specified in Stack_Container and must have a vector-like
 * interface (back, empty, push_back, pop_back, size are used).
 *
 * The stack_state_machine can hold onto a user defined context that is given on construction and can be
 * accessed through the context() member function.
 * This can (and is by default) be disabled by passing void to Context_Type, which suppresses the context()
 * member function and provides a parameter-less constructor instead.
 *
 * The state machine starts in an empty state where the stack is empty and an initial state must be entered.
 * This initial state can not be popped.
 */
template <typename Base_State, typename Context_Type, template <typename> class State_Policy,
		  template <typename...> class Stack_Container>
class stack_state_machine : public detail::stack_state_machine_context_wrapper<Context_Type> {
	using state_policy = State_Policy<stack_state_machine>;
	state_policy policy;
	Stack_Container<typename state_policy::owning_ptr_t> state_stack_;

public:
	using detail::stack_state_machine_context_wrapper<Context_Type>::stack_state_machine_context_wrapper;
	/// \brief Instructs the stack_state_machine to enter the new state defined by type State with the given
	/// arguments.
	template <typename State, typename... Args>
	void enter_state(Args&&... args) {
		if(current_state() != state_policy::ptr_t_empty) policy.leave_state_push(current_state());
		state_stack_.push_back(
				policy.template enter_state<State>(*this, current_state(), std::forward<Args>(args)...));
	}

	/// Pops the current state from the stack and reenters the previous one.
	/**
	 * If the current state is the only state on the stack, the pop is not performed and false is returned
	 * because the state machine my not return to being empty.
	 */
	template <typename... Args>
	bool pop_state(Args&&... args) {
		if(state_stack_.size() <= 1) return false;
		policy.leave_state_pop(current_state());
		state_stack_.pop_back();
		auto state = policy.get_ptr(state_stack_.back());
		policy.reenter_state(state, std::forward<Args>(args)...);
		return true;
	}

	/// \brief Allows access to the current state, returning a null-representation (ptr_t_empty in the policy)
	/// if the state machine has not entered a state yet.
	typename state_policy::ptr_t current_state() {
		if(state_stack_.empty()) {
			return state_policy::ptr_t_empty;
		} else {
			return policy.get_ptr(state_stack_.back());
		}
	}

#ifdef DOXYGEN
	/// \brief Constructs a stack_state_machine with the given context (Doesn't exist if Context_Type
	/// is void or a reference).
	stack_state_machine(const Context_Type& context);
	/// \brief Constructs a stack_state_machine with the given context (Only exists if Context_Type is a
	/// reference).
	stack_state_machine(Context_Type context);
	/// \brief Constructs a stack_state_machine without context (Only exists if Context_Type is void).
	stack_state_machine();

	/// \brief Allows access to the context variable held by the state machine (Doesn't exist if Context_Type
	/// is void or a reference).
	const Context_Type& context() const;
	/// \brief Allows access to the context variable held by the state machine (Doesn't exist if Context_Type
	/// is void or a reference).
	Context_Type& context();
	/// \brief Allows access to the context variable held by the state machine (Only exists if Context_Type is
	/// a reference).
	Context_Type context() const;
#endif
};

} // namespace util
} // namespace mce

#endif /* UTIL_STACK_STATE_MACHINE_HPP_ */
