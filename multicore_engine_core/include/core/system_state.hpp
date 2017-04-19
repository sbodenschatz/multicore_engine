/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/system_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_SYSTEM_STATE_HPP_
#define CORE_SYSTEM_STATE_HPP_

namespace mce {
namespace core {
class system;
struct frame_time;

class system_state {
protected:
	mce::core::system* system_;

public:
	system_state();
	virtual ~system_state();
	virtual void process(const mce::core::frame_time& frame_time);
	virtual void render(const mce::core::frame_time& frame_time);
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_SYSTEM_STATE_HPP_ */
