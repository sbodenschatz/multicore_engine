/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/model/polygon_model.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef MODEL_POLYGON_MODEL_HPP_
#define MODEL_POLYGON_MODEL_HPP_

/**
 * \file
 * Defines the representation of polygon models in the engine.
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

class model_manager;

/// Represents a static (3D-)polygon model consisting of arbitrarily many meshes used for rendering.
class polygon_model : public std::enable_shared_from_this<polygon_model> {
public:
	/// Represents the status of polygon_model.
	enum class state { loading, staging, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<polygon_model_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	static_model_meta_data meta_data_;

	void complete_loading(const asset::asset_ptr& polygon_asset, model_manager& mm) noexcept;
	void complete_staging(model_manager& mm) noexcept;

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

	friend class model_manager;

public:
	/// \brief Creates an model object with the given name. Should only be used within the model system but
	/// can't be private due to being used in make_shared.
	explicit polygon_model(const std::string& name);
	/// \brief Creates an model object with the given name. Should only be used within the model system but
	/// can't be private due to being used in make_shared.
	explicit polygon_model(std::string&& name);
	/// Forbids copy-construction of polygon_model.
	polygon_model(const polygon_model&) = delete;
	/// Forbids copy-assignment of polygon_model.
	polygon_model& operator=(const polygon_model&) = delete;
	/// \brief Instructs the model system to run the handler function object when the model has completed
	/// loading or to run the error_handler if an error occurred during loading.
	/**
	 * The handler function object must have the signature <code>void(const polygon_model_ptr&
	 * model)</code>.
	 * The error_handler function object must have the signature <code>void(std::exception_ptr)</code>.
	 * Both handlers are called either on the thread calling this function or on a worker thread of the asset
	 * system. Both must fit into their respective handler function wrapper type
	 * polygon_model_completion_handler and error_handler.
	 */
	template <typename F, typename E>
	void run_when_ready(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Polygon model '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					path_not_found_exception("Polygon model '" + name() + "' was cached as failed.")));
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
	/// Allows access to the meta data of the model.
	/**
	 * Requires the model to be ready for use.
	 */
	const static_model_meta_data& meta_data() const noexcept {
		return meta_data_;
	}
	/// Returns the name of the model.
	const std::string& name() const noexcept {
		return name_;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_POLYGON_MODEL_HPP_ */
