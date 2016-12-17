/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/collision_model.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_COLLISION_MODEL_HPP_
#define MODEL_COLLISION_MODEL_HPP_

#include <asset/asset_defs.hpp>
#include <atomic>
#include <exception>
#include <memory>
#include <model/model_defs.hpp>
#include <model/model_format.hpp>
#include <mutex>
#include <vector>

namespace mce {
namespace model {

class collision_model : public std::enable_shared_from_this<collision_model> {
public:
	enum class state { loading, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<collision_model_completion_handler> completion_handlers;
	std::vector<asset::error_handler> error_handlers;
	static_model_collision_data data_;

	void complete_loading(const asset::asset_ptr& collision_asset);

	void raise_error_flag(std::exception_ptr e) {
		current_state_ = state::error;
		std::unique_lock<std::mutex> lock(modification_mutex);
		for(auto& handler : error_handlers) {
			handler(e);
		}
		error_handlers.clear();
		completion_handlers.clear();
		error_handlers.shrink_to_fit();
		completion_handlers.shrink_to_fit();
	}

	friend class model_manager;

public:
	explicit collision_model(const std::string& name);
	explicit collision_model(std::string&& name);
	collision_model(const collision_model&) = delete;
	collision_model& operator=(const collision_model&) = delete;

	template <typename F, typename E>
	void run_when_loaded(F handler, E error_handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					std::runtime_error("Collision model '" + name() + "' was cached as failed.")));
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else if(current_state_ == state::error) {
			error_handler(std::make_exception_ptr(
					std::runtime_error("Collision model '" + name() + "' was cached as failed.")));
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
		if(current_state_ == state::error) throw std::runtime_error("Error loading model '" + name_ + "'.");
	}

	state current_state() const {
		return current_state_;
	}

	const static_model_collision_data& data() const {
		return data_;
	}

	const std::string& name() const {
		return name_;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_COLLISION_MODEL_HPP_ */
