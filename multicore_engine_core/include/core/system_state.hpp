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

/// Provides the base class for system_states holding game state specific data and functionality for a system.
class system_state {
protected:
	/// References the system for which data is held.
	mce::core::system* system_;

public:
	/// Constructs a system_state for the given system.
	explicit system_state(mce::core::system* system) : system_{system} {}
	/// Enables polymorphic destruction for system_state subclasses.
	virtual ~system_state() = default;
	/// Hook function called for the processing phase of a frame.
	virtual void process(const mce::core::frame_time& frame_time);
	/// Hook function called for the rendering phase of a frame.
	virtual void render(const mce::core::frame_time& frame_time);

	/// Allows access to the system to which this system state belongs.
	mce::core::system* system() const {
		return system_;
	}
};

} /* namespace core */
} /* namespace mce */

#endif /* CORE_SYSTEM_STATE_HPP_ */
