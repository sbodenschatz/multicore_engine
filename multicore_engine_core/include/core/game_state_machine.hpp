/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/game_state_machine.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_MACHINE_HPP_
#define CORE_GAME_STATE_MACHINE_HPP_

namespace mce {
namespace core {
class engine;
struct frame_time;

class game_state_machine {
	mce::core::engine* engine;

public:
	explicit game_state_machine(mce::core::engine* engine);
	~game_state_machine();

	void process(const mce::core::frame_time& frame_time);
	void render(const mce::core::frame_time& frame_time);
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_GAME_STATE_MACHINE_HPP_ */
