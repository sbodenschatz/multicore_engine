/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/model/collision_model.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef MODEL_COLLISION_MODEL_HPP_
#define MODEL_COLLISION_MODEL_HPP_

/**
 * \file
 * Defines the representation of a collision detection model.
 */

#include <atomic>
#include <exception>
#include <mce/asset/asset_defs.hpp>
#include <mce/exceptions.hpp>
#include <mce/model/model_defs.hpp>
#include <mce/model/model_format.hpp>
#include <mce/util/local_function.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace mce {
namespace model {

/// Represents a (3D-)model asset used for collision detection.
class collision_model : public std::enable_shared_from_this<collision_model> {
public:
	/// Represents the status of the model.
	enum class state { loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<collision_model_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	static_model_collision_data data_;

	void complete_loading(const asset::asset_ptr& collision_asset) noexcept;

	void raise_error_flag(std::exception_ptr e) noexcept {
		current_state_ = state::error;
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

	friend class model_data_manager;

public:
	/// \brief Creates an model object with the given name. Should only be used within the model system but
	/// can't be private due to being used in make_shared.
	explicit collision_model(const std::string& name);
	/// \brief Creates an model object with the given name. Should only be used within the model system but
	/// can't be private due to being used in make_shared.
	explicit collision_model(std::string&& name);
	/// Forbids copy-construction of collision_model.
	collision_model(const collision_model&) = delete;
	/// Forbids copy-assignment of collision_model.
	collision_model& operator=(const collision_model&) = delete;
	/// \brief Instructs the model system to run the handler function object when the model has completed
	/// loading or to run the error_handler if an error occurred during loading.
	/**
	 * The handler function object must have the signature <code>void(const collision_model_ptr&
	 * model)</code>.
	 * The error_handler function object must have the signature <code>void(std::exception_ptr)</code>.
	 * Both handlers are called either on the thread calling this function or on a worker thread of the asset
	 * system. Both must fit into their respective handler function wrapper type
	 * collision_model_completion_handler and error_handler.
	 */
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Collision model '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			lock.unlock();
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Collision model '" + name() + "' was cached as failed.")));
		} else {
			completion_handlers.emplace_back(std::move(handler));
			error_handlers.emplace_back(std::move(error_handler));
		}
	}
	/// Checks if the model is ready for use.
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
			throw path_not_found_exception("Error loading model '" + name_ + "'.");
	}
	/// Returns the current state of the model.
	state current_state() const noexcept {
		return current_state_;
	}
	/// Allows access to the collision data stored in the model.
	/**
	 * Requires the model to be ready for use.
	 */
	const static_model_collision_data& data() const noexcept {
		return data_;
	}
	/// Returns the name of the collision_model.
	const std::string& name() const noexcept {
		return name_;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_COLLISION_MODEL_HPP_ */
