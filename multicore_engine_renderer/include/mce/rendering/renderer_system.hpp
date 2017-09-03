/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_SYSTEM_HPP_
#define MCE_RENDERING_RENDERER_SYSTEM_HPP_

/**
 * \file
 * Defines the renderer_system class.
 */

#include <mce/core/system.hpp>
#include <mce/rendering/material_manager.hpp>
#include <mce/rendering/model_manager.hpp>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace graphics {
class graphics_system;
class sampler;
class descriptor_set_layout;
class pipeline_layout;
class render_pass;
} // namespace graphics
namespace rendering {

/// Provides the high-level rendering functionality for entities in a scene for the engine.
class renderer_system : public core::system {
	graphics::graphics_system& gs_;
	model_manager mdl_mgr;
	material_manager mat_mgr;

	std::shared_ptr<const graphics::sampler> default_sampler_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_scene_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_material_;
	std::shared_ptr<const graphics::pipeline_layout> pipeline_layout_scene_pass_;
	std::shared_ptr<const graphics::render_pass> main_render_pass_;

	friend class static_model_component;
	friend class technique;

public:
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1200;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1200;
	}

	/// Creates the renderer_system taking the graphics_system as a dependency.
	/**
	 * Should only be called in core::engine::add_system, but can't be made private and friended with engine
	 * because the construction takes place in std::make_unique.
	 */
	renderer_system(core::engine& eng, graphics::graphics_system& gs);
	/// Destroys the renderer_system and releases underlying resources.
	~renderer_system();

	/// Implements the hooked logic that happens at the beginning of the frame.
	void prerender(const mce::core::frame_time& frame_time) override;
	/// Implements the hooked logic that happens at the end of the frame.
	void postrender(const mce::core::frame_time& frame_time) override;
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
