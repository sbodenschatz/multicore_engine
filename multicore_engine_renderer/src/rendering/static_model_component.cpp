/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/static_model_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/renderer_state.hpp>
#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/static_model_component.hpp>

namespace mce {
namespace rendering {

static_model_component::static_model_component(renderer_state& state, entity::entity& owner,
											   const entity::component_configuration& conf)
		: component(owner, conf), state{state}, sys{*static_cast<renderer_system*>(state.system())} {}

static_model_component::~static_model_component() {}

void static_model_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY(prop, static_model_component, std::vector<std::string>, material_names);
	REGISTER_COMPONENT_PROPERTY(prop, static_model_component, std::string, model_name);
}

void static_model_component::material_names(const std::vector<std::string>& material_names) {
	material_names_ = material_names;
	materials_.clear();
	for(const auto& mat_name : material_names_) {
		materials_.push_back(sys.mat_mgr.load_material(mat_name));
	}
}

void static_model_component::model_name(const std::string& model_name) {
	model_name_ = model_name;
	model_ = sys.mdl_mgr.load_static_model(model_name);
}

} /* namespace rendering */
} /* namespace mce */
