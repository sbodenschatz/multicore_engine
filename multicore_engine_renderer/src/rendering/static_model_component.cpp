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
		: component(owner, conf), pending_callbacks{false}, state{state}, sys{*static_cast<renderer_system*>(
																				  state.system())} {}

static_model_component::~static_model_component() {
	std::unique_lock<std::mutex> lock(mtx);
	callback_cv.wait(lock, [this]() { return !pending_callbacks; });
}

void static_model_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY(prop, static_model_component, std::string, model_name);
}

void static_model_component::model_name(const std::string& model_name) {
	{
		std::unique_lock<std::mutex> lock(mtx);
		callback_cv.wait(lock, [this]() { return !pending_callbacks; });
		if(model_name.empty()) {
			model_name_ = "";
			model_ = nullptr;
			material_names_.clear();
			materials_.clear();
			return;
		}
		pending_callbacks = true;
	}
	sys.mdl_mgr.load_static_model(
			model_name,
			[this, model_name](const static_model_ptr& model) {
				std::vector<std::string> material_names;
				std::transform(model->meshes().begin(), model->meshes().end(),
							   std::back_inserter(material_names),
							   [](const static_model::mesh& msh) { return msh.material_name(); });
				std::vector<material_ptr> materials;
				for(const auto& mat_name : material_names) {
					materials.push_back(sys.mat_mgr.load_material(mat_name));
				}
				{
					std::lock_guard<std::mutex> lock(mtx);
					materials_ = materials;
					material_names_ = material_names;
					model_name_ = model_name;
					model_ = model;
					pending_callbacks = false;
				}
				callback_cv.notify_all();
			},
			[this](std::exception_ptr) {
				{
					std::lock_guard<std::mutex> lock(mtx);
					pending_callbacks = false;
				}
				callback_cv.notify_all();
			});
}

} /* namespace rendering */
} /* namespace mce */
