/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/asset.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_HPP_
#define ASSET_ASSET_HPP_

/**
 * \file
 * Defines the class representing assets in the engine.
 */

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/local_function.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace mce {
namespace asset {

/// Represents abstracted file-like data (with a name and content) used by the engine.
/**
 * An asset is a data object with a defined name that can be synchronously and asynchronously loaded from
 * different sources. It can be used by different parts of the engine that need data to work (assets can
 * contain e.g. models, textures, collision geometry or entity descriptions).
 */
class asset : public std::enable_shared_from_this<asset> {
public:
	/// Represents the current status of the asset.
	enum class state {
		/// The asset was requested but the loading has not been started yet (it is waiting in queue).
		initial,
		/// The asset is currently being loaded.
		loading,
		/// The asset is resident in memory and can be used.
		ready,
		/// An error prevented the asset from being loaded.
		error
	};

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
	/// \brief Creates an asset object with the given name. Should only be used within the asset system but
	/// can't be private due to being used in make_shared.
	explicit asset(const std::string& name);
	/// \brief Creates an asset object with the given name. Should only be used within the asset system but
	/// can't be private due to being used in make_shared.
	explicit asset(std::string&& name);
	/// Forbids copy-construction for asset.
	asset(const asset&) = delete;
	/// Forbids copy-assignment for asset.
	asset& operator=(const asset&) = delete;

	/// \brief Instructs the asset system to run the handler function object when the asset has completed
	/// loading or to run the error_handler if an error occurred during loading.
	/**
	 * The handler function object must have the signature <code>void(const asset_ptr& asset)</code>.
	 * The error_handler function object must have the signature <code>void(std::exception_ptr)</code>.
	 * Both handlers are called either on the thread calling this function or on a worker thread of the asset
	 * system. Both must fit into their respective handler function wrapper type asset_completion_handler and
	 * error_handler.
	 */
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
	/// Checks if the asset is ready for use.
	bool ready() const noexcept {
		return current_state_ == state::ready;
	}
	/// Checks if an error prevented loading.
	bool has_error() const noexcept {
		return current_state_ == state::error;
	}
	/// Triggers an error check by throwing an exception if an error prevented loading.
	void check_error_flag() const {
		if(current_state_ == state::error)
			throw path_not_found_exception("Error loading asset '" + name_ + "'.");
	}
	/// Returns the current state of the asset.
	state current_state() const noexcept {
		return current_state_;
	}
	/// Allows access to the content data when the asset is loaded.
	const char* data() const noexcept {
		return data_.get();
	}
	/// Returns a pointer to the content participating in ownership of the data block.
	const std::shared_ptr<const char>& data_shared() const noexcept {
		return data_;
	}
	/// Returns the name of the asset.
	const std::string& name() const noexcept {
		return name_;
	}
	/// Returns the size of the assets content when it is loaded.
	size_t size() const noexcept {
		return size_;
	}

	friend class asset_loader;
	friend class asset_manager;
	friend class dummy_asset;

private:
	void complete_loading(const std::shared_ptr<const char>& data, size_t size) noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept {
		current_state_ = state::error;
		completed_cv.notify_all();
		std::unique_lock<std::mutex> lock(modification_mutex);
		for(auto& handler : error_handlers) {
			try {
				handler(e);
			} catch(...) {
				// Drop exceptions escaped from completion handlers
			}
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

	bool try_obtain_load_ownership() noexcept {
		state expected = state::initial;
		return current_state_.compare_exchange_strong(expected, state::loading);
	}
};

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_HPP_ */
