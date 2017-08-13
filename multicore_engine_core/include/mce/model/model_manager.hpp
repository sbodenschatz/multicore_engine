/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/model/model_manager.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_MANAGER_HPP_
#define MODEL_MODEL_MANAGER_HPP_

/**
 * \file
 * Provides the functionality to load models into the engine.
 */

#include <boost/container/flat_map.hpp>
#include <mce/model/collision_model.hpp>
#include <mce/model/model_defs.hpp>
#include <mce/model/polygon_model.hpp>
#include <memory>
#include <shared_mutex>
#include <string>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset

namespace model {

/// Manages the loading and lifetime of collision and polygon models in the engine.
class model_manager {
public:
	/// Constructs a model_manager using the given asset_manager.
	explicit model_manager(asset::asset_manager& asset_manager) noexcept : asset_manager(asset_manager) {}

	/// \brief Asynchronously loads the polygon_model with the given name and calls the completion_handler
	/// function object on success or the error_handler function object on error.
	/**
	 * For the requirements for the function objects see polygon_model::run_when_ready.
	 */
	template <typename F, typename E>
	polygon_model_ptr load_polygon_model(const std::string& name, F completion_handler, E error_handler);
	/// Asynchronously loads the polygon_model with the given name.
	polygon_model_ptr load_polygon_model(const std::string& name);

	/// \brief Asynchronously loads the collision_model with the given name and calls the completion_handler
	/// function object on success or the error_handler function object on error.
	/**
	 * For the requirements for the function objects see collision_model::run_when_ready.
	 */
	template <typename F, typename E>
	collision_model_ptr load_collision_model(const std::string& name, F completion_handler, E error_handler);
	/// Asynchronously loads the collision_model with the given name.
	collision_model_ptr load_collision_model(const std::string& name);

private:
	asset::asset_manager& asset_manager;
	std::shared_timed_mutex loaded_polygon_models_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<polygon_model>> loaded_polygon_models;
	std::shared_timed_mutex loaded_collision_models_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<collision_model>> loaded_collision_models;

	std::shared_ptr<polygon_model> internal_load_polygon_model(const std::string& name);
	std::shared_ptr<collision_model> internal_load_collision_model(const std::string& name);

	void start_stage_polygon_model(const std::shared_ptr<polygon_model>& model) noexcept;

	friend class polygon_model;
};

template <typename F, typename E>
polygon_model_ptr model_manager::load_polygon_model(const std::string& name, F completion_handler,
													E error_handler) {
	auto model = internal_load_polygon_model(name);
	model->run_when_ready(std::move(completion_handler), std::move(error_handler));
	return model;
}
template <typename F, typename E>
collision_model_ptr model_manager::load_collision_model(const std::string& name, F completion_handler,
														E error_handler) {
	auto model = internal_load_collision_model(name);
	model->run_when_ready(std::move(completion_handler), std::move(error_handler));
	return model;
}

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_MANAGER_HPP_ */
