/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/static_model_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_
#define MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_

/**
 * \file
 * Defines the static_model_component class.
 */

#include <condition_variable>
#include <mce/entity/component.hpp>
#include <mce/rendering/rendering_defs.hpp>
#include <mutex>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {
class renderer_system;
class renderer_state;

/// Makes the model a object that is drawn into the 3D scene using a static_model to represent it.
/**
 * Due to this component class working asynchronously for model and material loading it is internally
 * thread-safe and has to block on destruction for pending callbacks to avoid use after destruction.
 */
class static_model_component : public entity::component {
	mutable std::mutex mtx;
	std::condition_variable callback_cv;
	bool pending_callbacks_;
	renderer_state& state;
	renderer_system& sys;
	std::string model_name_;
	static_model_ptr model_;
	std::vector<std::string> material_names_;
	std::vector<material_ptr> materials_;

public:
	/// \brief Creates a static_model_component for the given entity to attach to and the given
	/// component_configuration from which properties will be initialized.
	/**
	 * Additionally a renderer_state object is required for access to the model_manager and material_manager.
	 */
	static_model_component(renderer_state& state, entity::entity& owner,
						   const entity::component_configuration& conf);
	/// Destroys the static_model_component ensuring that there are no pending callbacks on it beforehand.
	~static_model_component();

	/// Returns the materials for the meshes of the model.
	std::vector<material_ptr> materials() const {
		std::lock_guard<std::mutex> lock(mtx);
		return materials_;
	}

	/// Returns the names of the materials for the meshes of the model.
	std::vector<std::string> material_names() const {
		std::lock_guard<std::mutex> lock(mtx);
		return material_names_;
	}

	/// Returns the model that should be drawn.
	const static_model_ptr& model() const {
		std::lock_guard<std::mutex> lock(mtx);
		return model_;
	}

	/// Returns the name of the model to draw.
	std::string model_name() const {
		std::lock_guard<std::mutex> lock(mtx);
		return model_name_;
	}

	/// Returns true if the static_model_component is ready for rendering.
	bool ready() const {
		std::lock_guard<std::mutex> lock(mtx);
		return bool(model_);
	}

	/// \brief Returns true if there are pending callbacks for model and material loading that will change
	/// model and material data asynchronously.
	bool pending_callbacks() const {
		std::lock_guard<std::mutex> lock(mtx);
		return pending_callbacks_;
	}

	/// \brief Asynchronously sets the model name, loads the the model with the given name and loads the
	/// materials associated with the model.
	/**
	 * \warning Due to the asynchronous nature of this setter, the results will not become visible immediately
	 * but only after the loading is complete. For the first assigned model ready() can be used to check if it
	 * is fully loaded. For all assignments pending_callbacks() can be used to check if there are incomplete.
	 * The state ready() == true, pending_callbacks() == true represents a subsequent assignment after the
	 * first, for which the change will become visible in the future. Until this happens the previous state
	 * will be visible.
	 */
	void model_name(const std::string& model_name);

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_ */
