/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/stack_state_machine_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <util/stack_state_machine.hpp>

namespace mce {
namespace util {

template <typename Base_State, typename State_Machine>
struct custom_policy {
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
	void leave_state_push(const ptr_t&) {}
	void leave_state_pop(const ptr_t&) {}
        template <typename... Args>
	void reenter_state(const ptr_t&,Args&&...) {}
};

struct test_context {
	int value = 42;
};

struct state {
	virtual ~state() {}
	virtual void leave_pop() = 0;
	virtual void leave_push() = 0;
	virtual void reenter() = 0;
	virtual char id() = 0;
};

typedef stack_state_machine<state, test_context,
							stack_state_machine_policy_binder<custom_policy, state>::policy>
		custom_policy_stack_state_machine;

struct state_A : public state {
	state_A(stack_state_machine<state>&, state*) {}
	state_A(stack_state_machine<state, test_context&>&, state*) {}
	state_A(stack_state_machine<state, test_context>&, state*) {}
	state_A(custom_policy_stack_state_machine&, state*) {}
	virtual void leave_pop(){};
	virtual void leave_push(){};
	virtual void reenter(){};
	virtual char id() {
		return 'A';
	}
};

struct state_B : public state {
	state_B(stack_state_machine<state>&, state*) {}
	state_B(stack_state_machine<state, test_context&>&, state*) {}
	state_B(stack_state_machine<state, test_context>&, state*) {}
	state_B(custom_policy_stack_state_machine&, state*) {}
	virtual void leave_pop(){};
	virtual void leave_push(){};
	virtual void reenter(){};
	virtual char id() {
		return 'B';
	}
};

TEST(util_stack_state_machine, basic) {
	stack_state_machine<state> state_machine;
	state_machine.enter_state<state_A>();
	ASSERT_EQ('A', state_machine.current_state()->id());
	state_machine.enter_state<state_B>();
	ASSERT_EQ('B', state_machine.current_state()->id());
	state_machine.pop_state();
	ASSERT_EQ('A', state_machine.current_state()->id());
}

TEST(util_stack_state_machine, with_reference_context) {
	test_context ctx;
	stack_state_machine<state, test_context&> state_machine(ctx);
	state_machine.enter_state<state_A>();
	ASSERT_EQ('A', state_machine.current_state()->id());
	state_machine.enter_state<state_B>();
	ASSERT_EQ('B', state_machine.current_state()->id());
	state_machine.pop_state();
	ASSERT_EQ('A', state_machine.current_state()->id());
	ASSERT_EQ(42, state_machine.context().value);
	ctx.value = 123;
	ASSERT_EQ(123, state_machine.context().value);
}

TEST(util_stack_state_machine, with_value_context) {
	test_context ctx;
	stack_state_machine<state, test_context> state_machine(ctx);
	state_machine.enter_state<state_A>();
	ASSERT_EQ('A', state_machine.current_state()->id());
	state_machine.enter_state<state_B>();
	ASSERT_EQ('B', state_machine.current_state()->id());
	state_machine.pop_state();
	ASSERT_EQ('A', state_machine.current_state()->id());
	ASSERT_EQ(42, state_machine.context().value);
	ctx.value = 123;
	ASSERT_EQ(42, state_machine.context().value);
}

TEST(util_stack_state_machine, with_custom_policy) {
	test_context ctx;
	custom_policy_stack_state_machine state_machine(ctx);
	state_machine.enter_state<state_A>();
	ASSERT_EQ('A', state_machine.current_state()->id());
	state_machine.enter_state<state_B>();
	ASSERT_EQ('B', state_machine.current_state()->id());
	state_machine.pop_state();
	ASSERT_EQ('A', state_machine.current_state()->id());
	ASSERT_EQ(42, state_machine.context().value);
	ctx.value = 123;
	ASSERT_EQ(42, state_machine.context().value);
}

} // namespace util
} // namespace mce
