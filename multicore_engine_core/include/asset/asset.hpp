/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_HPP_
#define ASSET_ASSET_HPP_

#include "asset_defs.hpp"
#include <atomic>
#include <condition_variable>
#include <exceptions.hpp>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <util/local_function.hpp>
#include <vector>

namespace mce {
namespace asset {

class asset : public std::enable_shared_from_this<asset> {
public:
	enum class state { initial, loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::shared_ptr<const char> data_;
	size_t size_;
	std::vector<asset_completion_handler> completion_handlers;
	std::vector<error_handler> error_handlers;
	mutable std::condition_variable completed_cv;

public:
	explicit asset(const std::string& name);
	explicit asset(std::string&& name);
	asset(const asset&) = delete;
	asset& operator=(const asset&) = delete;

	template <typename F, typename E>
	void run_when_loaded(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			// TODO: Maybe also run this asynchronously (post it into the thread pool of the asset manager)
			handler(std::static_pointer_cast<const asset>(this->shared_from_this()));
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Requested asset '" + name_ + "' is cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(std::static_pointer_cast<const asset>(this->shared_from_this()));
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Requested asset '" + name_ + "' is cached as failed.")));
		} else {
			completion_handlers.emplace_back(std::move(handler));
			error_handlers.emplace_back(std::move(error_handler));
		}
	}

	bool ready() const {
		return current_state_ == state::ready;
	}

	bool has_error() const {
		return current_state_ == state::error;
	}

	void check_error_flag() const {
		if(current_state_ == state::error)
			throw path_not_found_exception("Error loading asset '" + name_ + "'.");
	}

	state current_state() const {
		return current_state_;
	}

	const char* data() const {
		return data_.get();
	}

	const std::shared_ptr<const char>& data_shared() const {
		return data_;
	}

	const std::string& name() const {
		return name_;
	}

	size_t size() const {
		return size_;
	}

	friend class asset_loader;
	friend class asset_manager;

private:
	void complete_loading(const std::shared_ptr<const char>& data, size_t size);

	void raise_error_flag(std::exception_ptr e) {
		current_state_ = state::error;
		completed_cv.notify_all();
		std::unique_lock<std::mutex> lock(modification_mutex);
		for(auto& handler : error_handlers) {
			handler(e);
		}
		error_handlers.clear();
		completion_handlers.clear();
		error_handlers.shrink_to_fit();
		completion_handlers.shrink_to_fit();
	}

	// TODO Prevent a dead-lock where all workers are waiting (sync) for completions of assets whose load
	// tasks are further back in the thread pools queue (async).
	void internal_wait_for_complete() const {
		if(current_state_ == state::ready) return;
		check_error_flag();
		std::unique_lock<std::mutex> lock(modification_mutex);
		completed_cv.wait(lock, [this]() {
			auto cur_state = current_state_.load();
			return cur_state == state::ready || cur_state == state::error;
		});
		check_error_flag();
	}

	bool try_obtain_load_ownership() {
		state expected = state::initial;
		return current_state_.compare_exchange_strong(expected, state::loading);
	}
};

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_HPP_ */
