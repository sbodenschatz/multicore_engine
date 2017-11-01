/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/statistics.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_STATISTICS_HPP_
#define MCE_UTIL_STATISTICS_HPP_

/**
 * \file
 * Defines utilities to gather statistics in the engine or applications.
 */

#include <algorithm>
#include <atomic>

namespace mce {
namespace util {

/// Collects thread-safe aggregate statistics for a single variable of type T.
template <typename T>
class aggregate_statistic {
	struct state {
		T sum = T();
		T min = T();
		T max = T();
		size_t count = 0;

		state() noexcept {}
		state(const state& prev, const T& val) noexcept
				: sum{prev.sum + val}, min{std::min(prev.min, val)}, max{std::max(prev.max, val)},
				  count{prev.count + 1} {}
	};
	std::atomic<state> state_;

public:
	/// Records a sample for the variable.
	void record(const T& value) noexcept {
		auto s = state_.load();
		while(!state_.compare_exchange_weak(s, state(s, value)))
			;
	}

	/// Clears the statistics data for the variable.
	void clear() noexcept {
		state_.store(state());
	}

	/// Encapsulates a statistics evaluation result.
	template <typename Avg>
	struct result {
		Avg average;  ///< The arithmetic average of the samples.
		T sum;		  ///< The sum of all recorded samples.
		T minimum;	///< The minimal sample recorded.
		T maximum;	///< The maximal sample recorder.
		size_t count; ///< The number of recorder samples.

		/// Creates a result object for the given internal state.
		explicit result(const state& s) noexcept
				: average{Avg(s.sum) / s.count}, sum{s.sum}, minimum{s.min}, maximum{s.max}, count{s.count} {}
	};

	/// Evaluates the statistic of the samples recorded so far.
	/**
	 * Optionally uses the Avg type for the average value.
	 * This allows using a floating point value for the average of integer samples.
	 */
	template <typename Avg = T>
	result<Avg> evaluate() const noexcept {
		auto s = state_.load();
		return result<Avg>(s);
	}
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_STATISTICS_HPP_ */
