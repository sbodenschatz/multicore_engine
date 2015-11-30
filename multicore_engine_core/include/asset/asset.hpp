/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_HPP_
#define ASSET_ASSET_HPP_

#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <stdexcept>
#include <util/local_function.hpp>

namespace mce {
namespace asset {

class asset;

typedef std::shared_ptr<const asset> asset_ptr;

class asset : public std::enable_shared_from_this<asset> {
public:
	enum class state { initial, loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::shared_ptr<const char> data_;
	size_t size_;
	std::vector<util::local_function<128, void(const asset_ptr& asset)>> completion_handlers;
	mutable std::condition_variable completed_cv;

public:
	asset(const std::string& name);
	asset(const asset&) = delete;
	asset& operator=(const asset&) = delete;

	template <typename F>
	void run_when_loaded(F handler) {
		if(current_state_ == state::ready) {
			// TODO: Maybe also run this asynchronously (post it into the thread pool of the asset manager)
			handler(this->shared_from_this());
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else {
			completion_handlers.emplace_back(handler);
		}
	}

	bool ready() const {
		return current_state_ == state::ready;
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

	void raise_error_flag() {
		current_state_ = state::error;
		completed_cv.notify_all();
	}

	// TODO Prevent a dead-lock where all workers are waiting (sync) for completions of assets whose load
	// tasks are further back in the thread pools queue (async).
	void internal_wait_for_complete() const {
		if(current_state_ == state::ready) return;
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
