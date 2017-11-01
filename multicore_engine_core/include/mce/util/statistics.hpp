/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/statistics.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_STATISTICS_HPP_
#define MCE_UTIL_STATISTICS_HPP_

#include <algorithm>
#include <atomic>

namespace mce {
namespace util {

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
	void record(const T& value) noexcept {
		auto s = state_.load();
		while(!state_.compare_exchange_weak(s, state(s, value)))
			;
	}

	void clear() noexcept {
		state_.store(state());
	}

	template <typename Avg>
	struct result {
		Avg average;
		T sum;
		T minimum;
		T maximum;
		size_t count;

		explicit result(const state& s) noexcept
				: average{Avg(s.sum) / s.count}, sum{s.sum}, minimum{s.min}, maximum{s.max}, count{s.count} {}
	};

	template <typename Avg = T>
	result<Avg> evaluate() const noexcept {
		auto s = state_.load();
		return result<Avg>(s);
	}
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_STATISTICS_HPP_ */
