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

#include <cassert>
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
class framebuffer;
class pipeline;
} // namespace graphics
namespace rendering {

struct renderer_system_settings {
	std::string main_forward_vertex_shader_name = "shaders/main_forward.vert";
	std::string main_forward_fragment_shader_name = "shaders/main_forward.frag";
};

/// Provides the high-level rendering functionality for entities in a scene for the engine.
class renderer_system : public core::system {
	core::engine& eng_;
	graphics::graphics_system& gs_;
	renderer_system_settings settings_;
	rendering::model_manager mdl_mgr;
	rendering::material_manager mat_mgr;

	std::shared_ptr<const graphics::sampler> default_sampler_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_scene_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_material_;
	std::shared_ptr<const graphics::pipeline_layout> pipeline_layout_scene_pass_;
	std::shared_ptr<const graphics::render_pass> main_render_pass_;
	std::unique_ptr<graphics::framebuffer> main_framebuffer_;
	std::shared_ptr<const graphics::pipeline> main_forward_pipeline_;

	friend class static_model_component;
	friend class technique;

	void create_samplers();
	void create_descriptor_sets();
	void create_pipeline_layouts();
	void create_render_passes_and_framebuffers();
	void create_pipelines();

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
	renderer_system(core::engine& eng, graphics::graphics_system& gs, renderer_system_settings settings = {});
	/// Destroys the renderer_system and releases underlying resources.
	~renderer_system();

	/// Implements the hooked logic that happens at the beginning of the frame.
	void prerender(const mce::core::frame_time& frame_time) override;
	/// Implements the hooked logic that happens at the end of the frame.
	void postrender(const mce::core::frame_time& frame_time) override;

	const graphics::sampler& default_sampler() const {
		assert(default_sampler_);
		return *default_sampler_;
	}

	const std::shared_ptr<const graphics::descriptor_set_layout>& descriptor_set_layout_per_material() const {
		return descriptor_set_layout_per_material_;
	}

	const std::shared_ptr<const graphics::descriptor_set_layout>& descriptor_set_layout_per_scene() const {
		return descriptor_set_layout_per_scene_;
	}

	const graphics::pipeline& main_forward_pipeline() const {
		assert(main_forward_pipeline_);
		return *main_forward_pipeline_;
	}

	graphics::framebuffer& main_framebuffer() const {
		assert(main_framebuffer_);
		return *main_framebuffer_;
	}

	const graphics::render_pass& main_render_pass() const {
		assert(main_render_pass_);
		return *main_render_pass_;
	}

	const graphics::pipeline_layout& pipeline_layout_scene_pass() const {
		assert(pipeline_layout_scene_pass_);
		return *pipeline_layout_scene_pass_;
	}

	const renderer_system_settings& settings() const {
		return settings_;
	}

	const rendering::model_manager& model_manager() const {
		return mdl_mgr;
	}

	rendering::model_manager& model_manager() {
		return mdl_mgr;
	}

	const rendering::material_manager& material_manager() const {
		return mat_mgr;
	}

	rendering::material_manager& material_manager() {
		return mat_mgr;
	}
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
