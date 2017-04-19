/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/game_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_GAME_STATE_HPP_
#define CORE_GAME_STATE_HPP_

#include <vector>
#include <memory>

namespace mce {
namespace core {
class system_state;
struct frame_time;

class game_state {
protected:
	std::vector<std::unique_ptr<system_state>> system_states_;

public:
	game_state() = default;
	virtual ~game_state();

	void process(const mce::core::frame_time& frame_time);
	void render(const mce::core::frame_time& frame_time);

	virtual void preprocess(const mce::core::frame_time& frame_time);
	virtual void postprocess(const mce::core::frame_time& frame_time);
	virtual void prerender(const mce::core::frame_time& frame_time);
	virtual void postrender(const mce::core::frame_time& frame_time);

	virtual void leave_pop();
	virtual void leave_push();
	virtual void reenter();
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_GAME_STATE_HPP_ */
