/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_HPP_
#define ASSET_ASSET_HPP_

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
	std::atomic<bool> loaded; // true -> can read data unlocked. false -> synchronization is needed with other
							  // threads that register completion handlers and that load the asset.
	std::atomic<bool> error_flag;
	mutable std::mutex modification_mutex;
	std::string name;
	std::shared_ptr<const char> data;
	std::vector<util::local_function<128, void(const asset_ptr& asset)>> completion_handlers;
	mutable std::condition_variable completed_cv;

public:
	asset(const std::string& name);
	asset(const asset&) = delete;
	asset& operator=(const asset&) = delete;

	template <typename F>
	void run_when_loaded(F handler) {
		if(loaded) { handler(this->shared_from_this()); }
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(loaded) {
			lock.unlock();
			handler(this->shared_from_this());
		} else {
			completion_handlers.emplace_back(handler);
		}
	}

	void complete_loading(std::shared_ptr<const char> data);

	bool ready() const {
		return loaded;
	}

	void raise_error_flag() {
		error_flag = true;
	}

	bool has_error() const {
		return error_flag;
	}

	void wait_for_complete() const {
		if(loaded) return;
		std::unique_lock<std::mutex> lock(modification_mutex);
		completed_cv.wait(lock, [this]() { return loaded || error_flag; });
		check_error_flag();
	}
	void check_error_flag() const {
		if(error_flag) throw std::runtime_error("Error loading asset '" + name + "'.");
	}
};

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_HPP_ */
