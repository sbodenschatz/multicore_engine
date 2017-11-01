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
#include <limits>
#include <mce/containers/dynamic_array.hpp>
#include <vector>

namespace mce {
namespace util {

/// Collects thread-safe aggregate statistics for a single variable of type T.
template <typename T>
class aggregate_statistic {
	struct state {
		T sum = T();
		T min = std::numeric_limits<T>::max();
		T max = std::numeric_limits<T>::lowest();
		size_t count = 0;

		state() noexcept {}
		state(const state& prev, const T& val) noexcept
				: sum{prev.sum + val}, min{std::min(prev.min, val)}, max{std::max(prev.max, val)},
				  count{prev.count + 1} {}
	};
	std::atomic<state> state_ = state();

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

namespace detail {

template <typename T>
size_t histogram_bucket_index(const T& value, const T& lower, const T& upper, size_t bucket_count) noexcept {
	return size_t((bucket_count * (value - lower)) / (upper - lower));
}

template <typename T>
T histogram_bucket_lower_bound(size_t index, const T& lower, const T& upper, size_t bucket_count) noexcept {
	return T((index * (upper - lower)) / bucket_count + lower);
}

} // namespace detail

template <typename T>
class histogram_statistic {
	T lower_;
	T upper_;
	size_t bucket_count_;
	std::atomic<size_t> under_samples_ = 0;
	std::atomic<size_t> over_samples_ = 0;
	containers::dynamic_array<std::atomic<size_t>> hist_data_;

public:
	histogram_statistic(T lower, T upper, size_t bucket_count)
			: lower_{lower}, upper_{upper}, bucket_count_{bucket_count}, hist_data_(bucket_count, 0) {}

	void record(const T& value) noexcept {
		if(value < lower_) {
			++under_samples_;
		} else if(value >= upper_) {
			++over_samples_;
		} else {
			auto index = detail::histogram_bucket_index(value, lower_, upper_, bucket_count_);
			++hist_data_[index];
		}
	}

	void clear() noexcept {
		under_samples_.store(0);
		over_samples_.store(0);
		for(auto& h : hist_data_) {
			h.store(0);
		}
	}

	struct histogram_result {
		size_t under_samples;
		size_t over_samples;
		struct bucket {
			T lower_bound;
			T upper_bound;
			size_t samples;
			bucket(T lower_bound, T upper_bound, size_t samples)
					: lower_bound{lower_bound}, upper_bound{upper_bound}, samples{samples} {}
		};
		std::vector<bucket> buckets;
	};

	histogram_result evaluate() const {
		histogram_result res{under_samples_.load(), over_samples_.load(), {}};
		for(size_t i = 0; i < hist_data_.size(); ++i) {
			auto next = i + 1;
			auto lower = detail::histogram_bucket_lower_bound(i, lower_, upper_, bucket_count_);
			auto upper = detail::histogram_bucket_lower_bound(next, lower_, upper_, bucket_count_);
			res.buckets.emplace_back(lower, upper, hist_data_[i].load());
		}
		return res;
	}
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_STATISTICS_HPP_ */
