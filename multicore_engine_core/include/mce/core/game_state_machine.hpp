/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/game_state_machine.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_MACHINE_HPP_
#define CORE_GAME_STATE_MACHINE_HPP_

#include <boost/any.hpp>
#include <mce/util/stack_state_machine.hpp>

namespace mce {
namespace core {
class engine;
struct frame_time;
class game_state;
class game_state_machine;

namespace detail {

template <typename State_Machine>
struct game_state_machine_policy
		: public util::stack_state_machine_default_policy<game_state, State_Machine> {
	template <typename T, typename... Args>
	typename game_state_machine_policy::owning_ptr_t
	enter_state(State_Machine& state_machine, const typename game_state_machine_policy::ptr_t& parent_state,
				Args&&... args) {
		return std::make_unique<T>(state_machine.context().engine, state_machine.context().game_state_machine,
								   parent_state, std::forward<Args>(args)...);
	}
	static constexpr bool pop_states_on_destruction = true;
};

struct game_state_machine_context {
	mce::core::engine* engine;
	mce::core::game_state_machine* game_state_machine;
};

} // namespace detail

/// Implements the stack state machine for game states.
class game_state_machine {
	mce::core::engine* engine;
	util::stack_state_machine<game_state, detail::game_state_machine_context> state_machine;

public:
	/// Constructs a game_state_machine for the given engine object.
	explicit game_state_machine(mce::core::engine* engine);
	/// Destroys the game_state_machine.
	~game_state_machine();

	/// Handles the state specific part of the processing phase by delegating to the current state.
	void process(const mce::core::frame_time& frame_time);
	/// Handles the state specific part of the rendering phase by delegating to the current state.
	void render(const mce::core::frame_time& frame_time);

	/// Enters the game state represented by the given state class by constructing an object of it.
	/**
	 * The class must be constructible with the following argument list:
	 *  - mce::core::engine * : Pointer to the engine object owning the state machine.
	 *  - mce::core::game_state_machine * : Pointer to this state machine object.
	 *  - mce::core::game_state * : Pointer to the parent game_state (the state from which the engine
	 * transitioned into the new one).
	 *  - forwarded Args... : The unpacked supplied variadic arguments.
	 */
	template <typename State, typename... Args>
	void enter(Args&&... args) {
		state_machine.enter_state<State>(std::forward<Args>(args)...);
	}

	/// Pops the current state from the stack and returns to the state below it, if possible.
	/**
	 * If the current state is the only state on the stack, it can't be left or popped. In this case the
	 * function doesn't change the state and returns false.
	 * Otherwise true is returned.
	 *
	 * Arbitrary parameter data can be passed using an any object. What parameter types are expected /
	 * accepted depends on the game_state that is below the current one on the stack.
	 */
	bool pop(const boost::any& parameters);
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_GAME_STATE_MACHINE_HPP_ */
