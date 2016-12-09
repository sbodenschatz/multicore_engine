/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/model.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_POLYGON_MODEL_HPP_
#define MODEL_POLYGON_MODEL_HPP_

#include <asset/asset_defs.hpp>
#include <atomic>
#include <memory>
#include <model/model_defs.hpp>
#include <model/model_format.hpp>
#include <mutex>
#include <vector>

namespace mce {
namespace model {

class polygon_model : public std::enable_shared_from_this<polygon_model> {
public:
	enum class state { loading, staging, ready, error };

private:
	std::atomic<state> current_state_;
	mutable std::mutex modification_mutex;
	std::string name_;
	std::vector<polygon_model_completion_handler> completion_handlers;
	static_model_meta_data meta_data_;

	void complete_loading(const asset::asset_ptr& polygon_asset);
	void complete_staging();

	void raise_error_flag() {
		current_state_ = state::error;
	}

	friend class model_manager;

public:
	explicit polygon_model(const std::string& name);
	explicit polygon_model(std::string&& name);
	polygon_model(const polygon_model&) = delete;
	polygon_model& operator=(const polygon_model&) = delete;

	template <typename F>
	void run_when_ready(F handler) {
		if(current_state_ == state::ready) {
			handler(this->shared_from_this());
			return;
		}
		std::unique_lock<std::mutex> lock(modification_mutex);
		if(current_state_ == state::ready) {
			lock.unlock();
			handler(this->shared_from_this());
		} else {
			completion_handlers.emplace_back(std::move(handler));
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

	const static_model_meta_data& meta_data() const {
		return meta_data_;
	}

	const std::string& name() const {
		return name_;
	}
};

} // namespace model
} // namespace mce

#endif /* MODEL_POLYGON_MODEL_HPP_ */
