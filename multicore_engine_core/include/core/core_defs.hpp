/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/core/core_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_CORE_DEFS_HPP_
#define CORE_CORE_DEFS_HPP_

namespace mce {
namespace core {

/// Represents timing information for a frame.
struct frame_time {
	float delta_t; ///< The time step (duration of a frame).
};

} // namespace core
} // namespace mce

#endif /* CORE_CORE_DEFS_HPP_ */
