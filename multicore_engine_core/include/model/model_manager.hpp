/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/model/model_loader.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef MODEL_MODEL_MANAGER_HPP_
#define MODEL_MODEL_MANAGER_HPP_

#include <boost/container/flat_map.hpp>
#include <model/collision_model.hpp>
#include <model/model_defs.hpp>
#include <model/polygon_model.hpp>
#include <shared_mutex>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset

namespace model {

class model_manager {
public:
	model_manager(asset::asset_manager& asset_manager) noexcept : asset_manager(asset_manager) {}

	template <typename F>
	polygon_model_ptr load_polygon_model(const std::string& name, F completion_handler);
	polygon_model_ptr load_polygon_model(const std::string& name);

	template <typename F>
	collision_model_ptr load_collision_model(const std::string& name, F completion_handler);
	collision_model_ptr load_collision_model(const std::string& name);

private:
	asset::asset_manager& asset_manager;
	std::shared_timed_mutex loaded_polygon_models_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<polygon_model>> loaded_polygon_models;
	std::shared_timed_mutex loaded_collision_models_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<collision_model>> loaded_collision_models;

	std::shared_ptr<polygon_model> internal_load_polygon_model(const std::string& name);
	std::shared_ptr<collision_model> internal_load_collision_model(const std::string& name);

	void start_stage_polygon_model(std::shared_ptr<polygon_model> model);
};

template <typename F>
polygon_model_ptr model_manager::load_polygon_model(const std::string& name, F completion_handler) {
	auto model = internal_load_polygon_model(name);
	model->run_when_ready(std::move(completion_handler));
	return model;
}
template <typename F>
collision_model_ptr model_manager::load_collision_model(const std::string& name, F completion_handler) {
	auto model = internal_load_collision_model(name);
	model->run_when_loaded(std::move(completion_handler));
	return model;
}

} // namespace model
} // namespace mce

#endif /* MODEL_MODEL_MANAGER_HPP_ */
