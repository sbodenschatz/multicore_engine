/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/core_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_CORE_DEFS_HPP_
#define CORE_CORE_DEFS_HPP_

#include <chrono>

namespace mce {
namespace core {

/// Represents timing information for a frame.
struct frame_time {
	float delta_t; ///< The time step (duration of a frame).
	std::chrono::microseconds delta_t_microseconds;
	std::chrono::microseconds t;
};

} // namespace core
} // namespace mce

#endif /* CORE_CORE_DEFS_HPP_ */
