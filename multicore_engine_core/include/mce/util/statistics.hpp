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
#include <boost/container/flat_map.hpp>
#include <limits>
#include <mce/containers/dynamic_array.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/type_id.hpp>
#include <memory>
#include <mutex>
#include <ostream>
#include <shared_mutex>
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
	std::atomic<state> state_ = {state()};

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
				: average{Avg(Avg(s.sum) / s.count)}, sum{s.sum}, minimum{s.min}, maximum{s.max},
				  count{s.count} {}

		/// Allows outputting the result data to an ostream.
		friend std::ostream& operator<<(std::ostream& ostr, const result& res) {
			ostr << res.average << ";" << res.sum << ";" << res.minimum << ";" << res.maximum << ";"
				 << res.count << "\n";
			return ostr;
		}
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
	std::atomic<size_t> under_samples_ = {0};
	std::atomic<size_t> over_samples_ = {0};
	containers::dynamic_array<std::atomic<size_t>> hist_data_;

public:
	histogram_statistic(T lower, T upper, size_t bucket_count)
			: lower_{lower}, upper_{upper}, bucket_count_{bucket_count}, hist_data_(bucket_count, 0) {}

	/// Records a sample for the variable.
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

	/// Clears the statistics data for the variable.
	/**
	 * \warning Clearing is not performed atomically but in a thread-safe manner.
	 */
	void clear() noexcept {
		under_samples_.store(0);
		over_samples_.store(0);
		for(auto& h : hist_data_) {
			h.store(0);
		}
	}

	/// Encapsulates a statistics evaluation result.
	struct result {
		size_t under_samples = 0; ///< The number of samples smaller than the lower bound.
		size_t over_samples = 0;  ///< The number of samples larger or equal to the upper bound.
		/// Represents a histogram bucket in a result.
		struct bucket {
			T lower_bound;  ///< The lower bound of the bucket (approximated to T's precision).
			T upper_bound;  ///< The upper bound of the bucket (approximated to T's precision).
			size_t samples; ///< The number of samples in the bucket.
			/// Creates a bucket description.
			bucket(T lower_bound, T upper_bound, size_t samples)
					: lower_bound{lower_bound}, upper_bound{upper_bound}, samples{samples} {}
		};
		std::vector<bucket> buckets; ///< Represents the buckets in the result.

		/// Allows outputting the result data to an ostream.
		friend std::ostream& operator<<(std::ostream& ostr, const result& res) {
			if(res.under_samples) {
				if(!res.buckets.empty()) {
					ostr << std::numeric_limits<T>::lowest() << ";" << res.buckets.front().lower_bound;
				} else {
					ostr << ";";
				}
				ostr << ";" << res.under_samples << "\n";
			}
			for(const auto& b : res.buckets) {
				ostr << b.lower_bound << ";" << b.upper_bound << ";" << b.samples << "\n";
			}
			if(res.over_samples) {
				if(!res.buckets.empty()) {
					ostr << res.buckets.back().upper_bound << ";" << std::numeric_limits<T>::max();
				} else {
					ostr << ";";
				}
				ostr << ";" << res.under_samples << "\n";
			}
			return ostr;
		}
	};

	/// Evaluates the statistic of the samples recorded so far.
	/**
	 * \warning The evaluation is not performed atomically but in a thread-safe manner.
	 */
	result evaluate() const {
		result res{under_samples_.load(), over_samples_.load(), {}};
		for(size_t i = 0; i < hist_data_.size(); ++i) {
			auto next = i + 1;
			auto lower = detail::histogram_bucket_lower_bound(i, lower_, upper_, bucket_count_);
			auto upper = detail::histogram_bucket_lower_bound(next, lower_, upper_, bucket_count_);
			res.buckets.emplace_back(lower, upper, hist_data_[i].load());
		}
		return res;
	}
};

namespace detail {

struct statistics_container_base {
	type_id_t type_;
	explicit statistics_container_base(type_id_t type) : type_{type} {}
	virtual ~statistics_container_base() noexcept = default;
	virtual void write_result_to(std::ostream& ostr) noexcept = 0;
	virtual void clear() noexcept = 0;
};

template <typename Stat>
struct statistics_container : statistics_container_base {
	Stat stat;
	template <typename... Args>
	explicit statistics_container(Args&&... args)
			: statistics_container_base(type_id<statistics_container_base>::id<Stat>()),
			  stat(std::forward<Args>(args)...) {}
	virtual void write_result_to(std::ostream& ostr) noexcept override {
		auto r = stat.evaluate();
		ostr << r;
	}
	virtual void clear() noexcept override {
		stat.clear();
	}
};

} // namespace detail

/// Provides a name-based directory for statistics objects with functionality to centrally save or clear them.
class statistics_manager {
	mutable std::shared_timed_mutex mtx;
	boost::container::flat_map<std::string, std::shared_ptr<detail::statistics_container_base>> stats_;

public:
	/// \brief Creates, registers and returns (by shared_ptr) a statistics object of type Stat with the given
	/// name and constructor parameters.
	/**
	 * If an object with the name already exists and is of he request type, this object is returned instead.
	 * If an object with the given name but a different type exists an exception is thrown.
	 */
	template <typename Stat, typename... Args>
	std::shared_ptr<Stat> create(const std::string& name, Args&&... args) {
		std::unique_lock<std::shared_timed_mutex> lock(mtx);
		auto it = stats_.find(name);
		if(it == stats_.end()) {
			auto sptr = std::make_shared<detail::statistics_container<Stat>>(std::forward<Args>(args)...);
			stats_.emplace(name, sptr);
			auto ptr = static_cast<detail::statistics_container<Stat>*>(sptr.get());
			return std::shared_ptr<Stat>(sptr, &(ptr->stat));
		} else if(it->second->type_ == type_id<detail::statistics_container_base>::id<Stat>()) {
			auto ptr = static_cast<detail::statistics_container<Stat>*>(it->second.get());
			return std::shared_ptr<Stat>(it->second, &(ptr->stat));
		} else {
			throw mce::key_already_used_exception(
					"The given name is already in use for a statistic of a different type.");
		}
	}

	/// \brief Obtains a shared_ptr to the statistics object with the given name if it exists and is of the
	/// requested type or returns an empty pointer if it doesn't exist or is of a different type.
	template <typename Stat>
	std::shared_ptr<Stat> get(const std::string& name) const {
		std::shared_lock<std::shared_timed_mutex> lock(mtx);
		auto it = stats_.find(name);
		if(it == stats_.end()) {
			return std::shared_ptr<Stat>();
		} else if(it->second->type_ == type_id<detail::statistics_container_base>::id<Stat>()) {
			auto ptr = static_cast<detail::statistics_container<Stat>*>(it->second.get());
			return std::shared_ptr<Stat>(it->second, &(ptr->stat));
		} else {
			return std::shared_ptr<Stat>();
		}
	}

	/// Saves all registered objects to CSV files with the name of each object.
	void save() const;
	/// Clears the values of all registered statistics objects.
	void clear_values();
	/// Removes all registered statistics objects.
	void clear();
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_STATISTICS_HPP_ */
