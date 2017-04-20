/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/game_state_machine.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_MACHINE_HPP_
#define CORE_GAME_STATE_MACHINE_HPP_

#include <util/stack_state_machine.hpp>
#include <boost/any.hpp>

namespace mce {
namespace core {
class engine;
struct frame_time;
class game_state;

/// Implements the stack state machine for game states.
class game_state_machine {
	mce::core::engine* engine;
	util::stack_state_machine<game_state, mce::core::engine*> state_machine;

public:
	/// Constructs a game_state_machine for the given engine object.
	explicit game_state_machine(mce::core::engine* engine);
	/// Destroys the game_state_machine.
	~game_state_machine();

	/// Handles the state specific part of the processing phase by delegating to the current state.
	void process(const mce::core::frame_time& frame_time);
	/// Handles the state specific part of the rendering phase by delegating to the current state.
	void render(const mce::core::frame_time& frame_time);

	/// \brief Enters the game state represented by the given state class by constructing it using the given
	/// parameters.
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
