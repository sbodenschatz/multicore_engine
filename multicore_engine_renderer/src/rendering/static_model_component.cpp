/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/static_model_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/static_model_component.hpp>

namespace mce {
namespace rendering {

static_model_component::static_model_component(renderer_system& sys, entity::entity& owner,
											   entity::component_configuration& conf)
		: component(owner, conf), sys{sys} {
	// TODO Auto-generated constructor stub
}

static_model_component::~static_model_component() {}

void static_model_component::material_name(const std::string& material_name) {
	material_name_ = material_name;
	sys.mat_mgr.load_material(material_name);
}

void static_model_component::model_name(const std::string& model_name) {
	model_name_ = model_name;
	sys.mdl_mgr.load_static_model(model_name);
}

} /* namespace rendering */
} /* namespace mce */
