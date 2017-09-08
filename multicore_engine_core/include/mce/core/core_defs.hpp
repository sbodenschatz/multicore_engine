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
	std::chrono::microseconds t_microseconds;
};

class clock {
	using engine_clock_t =
			std::conditional<std::chrono::high_resolution_clock::is_steady,
							 std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;
	static_assert(std::ratio_less_equal<engine_clock_t::period, std::micro>::value,
				  "Standard library doesn't provide a steady clock with at least microsecond resolution.");

	engine_clock_t::time_point start_t;
	engine_clock_t::time_point old_t;

public:
	clock() noexcept {
		start_t = engine_clock_t::now();
		old_t = start_t;
	}

	std::chrono::microseconds microseconds_from_start() const noexcept {
		auto now_t = engine_clock_t::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(now_t - start_t);
	}

	static engine_clock_t::time_point now() noexcept {
		return engine_clock_t::now();
	}

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
