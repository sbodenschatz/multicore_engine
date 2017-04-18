/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/stack_state_machine_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <util/stack_state_machine.hpp>

namespace mce {
namespace util {

struct state {
	virtual ~state() {}
	virtual void leave_pop() = 0;
	virtual void leave_push() = 0;
	virtual void reenter() = 0;
	virtual char id() = 0;
};

struct state_A : public state {
	state_A(mce::util::stack_state_machine<mce::util::state, void*>&, void*, mce::util::state*) {}
	virtual void leave_pop(){};
	virtual void leave_push(){};
	virtual void reenter(){};
	virtual char id() {
		return 'A';
	}
};

struct state_B : public state {
	state_B(mce::util::stack_state_machine<mce::util::state, void*>&, void*, mce::util::state*) {}
	virtual void leave_pop(){};
	virtual void leave_push(){};
	virtual void reenter(){};
	virtual char id() {
		return 'B';
	}
};

TEST(util_stack_state_machine, basic_positive) {
	stack_state_machine<state, void*> state_machine(nullptr);
	state_machine.enter_state<state_A>();
	ASSERT_EQ('A', state_machine.current_state()->id());
	state_machine.enter_state<state_B>();
	ASSERT_EQ('B', state_machine.current_state()->id());
	state_machine.pop_state();
	ASSERT_EQ('A', state_machine.current_state()->id());
}

} // namespace util
} // namespace mce
