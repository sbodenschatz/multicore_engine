/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/load_unit.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_LOAD_UNIT_HPP_
#define ASSET_LOAD_UNIT_HPP_

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <vector>
#include <util/local_function.hpp>
#include "load_unit_meta_data.hpp"
#include "asset_defs.hpp"

namespace mce {
namespace asset {

class load_unit;

class load_unit : public std::enable_shared_from_this<load_unit> {
public:
	enum class state {
		initial,	  // Created, not yet loading
		meta_loading, // Loader thread associated, loading meta data
		meta_ready,   // Meta data loaded, not yet loading pay load data
		data_loading, // Meta data loaded, loading pay load data
		data_ready,   // Loading complete
		error		  // Loading failed
	};

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	load_unit_meta_data meta_data_;
	std::shared_ptr<const char> payload_data_;
	size_t size_;
	std::vector<load_unit_completion_handler> completion_handlers;
	std::vector<simple_completion_handler> simple_completion_handlers;
	mutable std::condition_variable completed_cv;

public:
	class asset_resolution_cookie {
		friend class load_unit;
		const mce::asset::load_unit* load_unit;
		uint64_t offset;
		uint64_t size;

	public:
		operator bool() const {
			return load_unit != nullptr;
		}
		asset_resolution_cookie() : load_unit{nullptr}, offset{0}, size{0} {}
		asset_resolution_cookie(const mce::asset::load_unit* load_unit, uint64_t offset, uint64_t size)
				: load_unit{load_unit}, offset{offset}, size{size} {}
	};
	explicit load_unit(const std::string& name);
	explicit load_unit(std::string&& name);
	load_unit(const load_unit&) = delete;
	load_unit& operator=(const load_unit&) = delete;

	asset_resolution_cookie resolve_asset(const std::string& name) const;
	std::pair<std::shared_ptr<const char>, size_t>
	get_asset_content(const asset_resolution_cookie& resolution_cookie) const;

	template <typename F>
	void run_when_loaded(F handler) {
		if(current_state_ == state::data_ready) {
			// TODO: Maybe also run this asynchronously (post it into the thread pool of the asset manager)
			handler(this->shared_from_this());
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::data_ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else {
			completion_handlers.emplace_back(handler);
		}
	}

	template <typename F>
	void run_when_loaded_simple(F handler) {
		if(current_state_ == state::data_ready) {
			// TODO: Maybe also run this asynchronously (post it into the thread pool of the asset manager)
			handler();
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::data_ready) {
			lock.unlock();
			handler();
		} else {
			simple_completion_handlers.emplace_back(handler);
		}
	}

	bool meta_data_ready() const {
		auto cur_state = current_state_.load();
		return cur_state == state::meta_ready || cur_state == state::data_loading ||
			   cur_state == state::data_ready;
	}

	bool ready() const {
		return current_state_ == state::data_ready;
	}

	bool has_error() const {
		return current_state_ == state::error;
	}

	void check_error_flag() const {
		if(current_state_ == state::error) throw std::runtime_error("Error loading asset '" + name_ + "'.");
	}

	state current_state() const {
		return current_state_;
	}

	const std::string& name() const {
		return name_;
	}

	friend class load_unit_asset_loader;

private:
	void load_meta_data(std::shared_ptr<const char> data, size_t size);
	void complete_loading(std::shared_ptr<const char> data, size_t size);

	void raise_error_flag() {
		current_state_ = state::error;
		completed_cv.notify_all();
	}

	// TODO Prevent a dead-lock where all workers are waiting (sync) for completions of assets whose load
	// tasks are further back in the thread pools queue (async).
	void internal_wait_for_data_complete() const {
		if(ready()) return;
		check_error_flag();
		std::unique_lock<std::mutex> lock(modification_mutex);
		completed_cv.wait(lock, [this]() {
			auto cur_state = current_state_.load();
			return cur_state == state::data_ready || cur_state == state::error;
		});
		check_error_flag();
	}

	// TODO Prevent a dead-lock where all workers are waiting (sync) for completions of assets whose load
	// tasks are further back in the thread pools queue (async).
	void internal_wait_for_meta_complete() const {
		if(meta_data_ready()) return;
		check_error_flag();
		std::unique_lock<std::mutex> lock(modification_mutex);
		completed_cv.wait(lock, [this]() {
			auto cur_state = current_state_.load();
			return cur_state == state::meta_ready || cur_state == state::data_loading ||
				   cur_state == state::data_ready || cur_state == state::error;
		});
		check_error_flag();
	}

	bool try_obtain_meta_load_ownership() {
		state expected = state::initial;
		return current_state_.compare_exchange_strong(expected, state::meta_loading);
	}

	bool try_obtain_data_load_ownership() {
		state expected = state::meta_ready;
		return current_state_.compare_exchange_strong(expected, state::data_loading);
	}
};

} // namespace asset
} // namespace mce

#endif /* ASSET_LOAD_UNIT_HPP_ */
