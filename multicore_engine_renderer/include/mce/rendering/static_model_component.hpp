/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/static_model_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_
#define MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_

#include <mce/entity/component.hpp>
#include <mce/rendering/rendering_defs.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {
class renderer_system;
class renderer_state;

class static_model_component : public entity::component {
	renderer_state& state;
	renderer_system& sys;
	std::string model_name_;
	static_model_ptr model_;
	std::string material_name_;
	material_ptr material_;

public:
	static_model_component(renderer_state& state, entity::entity& owner,
						   const entity::component_configuration& conf);
	~static_model_component();

	const material_ptr& material() const {
		return material_;
	}

	std::string material_name() const {
		return material_name_;
	}

	const static_model_ptr& model() const {
		return model_;
	}

	std::string model_name() const {
		return model_name_;
	}

	void material_name(const std::string& material_name);

	void model_name(const std::string& model_name);

	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_ */
