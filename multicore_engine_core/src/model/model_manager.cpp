/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/model_manager.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <model/model_manager.hpp>
#include <util/unused.hpp>

namespace mce {
namespace model {

polygon_model_ptr model_manager::load_polygon_model(const std::string& name) {
	return internal_load_polygon_model(name);
}
collision_model_ptr model_manager::load_collision_model(const std::string& name) {
	return internal_load_collision_model(name);
}
std::shared_ptr<polygon_model> model_manager::internal_load_polygon_model(const std::string& name) {
	// TODO: Implement
	UNUSED(name);
	return nullptr;
}
std::shared_ptr<collision_model> model_manager::internal_load_collision_model(const std::string& name) {
	// TODO: Implement
	UNUSED(name);
	return nullptr;
}

} // namespace model
} // namespace mce
