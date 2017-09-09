/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/core_defs.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CORE_CORE_DEFS_HPP_
#define CORE_CORE_DEFS_HPP_

/**
 * \file
 * Provides miscellaneous definitions for the core namespace.
 */

#include <chrono>

namespace mce {
namespace core {

/// Represents timing information for a frame.
struct frame_time {
	float delta_t; ///< The time step (duration of a frame) mesured in seconds.
	std::chrono::microseconds delta_t_microseconds; ///<The time step in milliseconds.
	/// The time since the start of the corresponding clock in milliseconds.
	std::chrono::microseconds t_microseconds;
};

/// Provides time measurement functionality using steady time for the engine.
/**
 * The clock is started at construction and can measure the elapsed time and can measure the time between
 * calls to frame_tick().
 */
class clock {
	using engine_clock_t =
			std::conditional<std::chrono::high_resolution_clock::is_steady,
							 std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;
	static_assert(std::ratio_less_equal<engine_clock_t::period, std::micro>::value,
				  "Standard library doesn't provide a steady clock with at least microsecond resolution.");

	engine_clock_t::time_point start_t;
	engine_clock_t::time_point old_t;

public:
	/// Constructs and starts the clock.
	clock() noexcept {
		start_t = engine_clock_t::now();
		old_t = start_t;
	}

	/// Returns the microseconds that have passed since the clock object was started.
	std::chrono::microseconds microseconds_from_start() const noexcept {
		auto now_t = engine_clock_t::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(now_t - start_t);
	}

	/// Returns the current time_point from the underlying steady clock.
	static engine_clock_t::time_point now() noexcept {
		return engine_clock_t::now();
	}

	/// Measures the time since the last call to frame_tick() (or from the start on the first call).
	/**
	 * The time difference (delta_t) is provided as a float measuring seconds and as a
	 * std::chrono::microseconds value.
	 * Additionally the time (t) in milliseconds from the start is also provided.
	 */
	frame_time frame_tick() noexcept {
		auto new_t = engine_clock_t::now();
		auto t_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(new_t - start_t);
		auto delta_t_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(new_t - old_t);
		std::chrono::duration<float> delta_t = new_t - old_t;
		old_t = new_t;
		return {delta_t.count(), delta_t_microseconds, t_microseconds};
	}
};

} // namespace core
} // namespace mce

#endif /* CORE_CORE_DEFS_HPP_ */
