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

#include <boost/optional.hpp>
#include <cassert>
#include <mce/containers/dynamic_array.hpp>
#include <mce/containers/per_thread.hpp>
#include <mce/core/system.hpp>
#include <mce/graphics/command_pool.hpp>
#include <mce/graphics/simple_descriptor_pool.hpp>
#include <mce/graphics/simple_uniform_buffer.hpp>
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

/// Encapsulates settings of the renderer that are tweakable by the application using the engine.
/**
 * The settings are provided to the renderer_system as an optional constructor parameter.
 */
struct renderer_system_settings {
	/// Specifies the name of the vertex shader to use for the main forward rendering pass.
	std::string main_forward_vertex_shader_name = "shaders/main_forward.vert";
	/// Specifies the name of the fragment shader to use for the main forward rendering pass.
	std::string main_forward_fragment_shader_name = "shaders/main_forward.frag";
};

/// Provides the high-level rendering functionality for entities in a scene for the engine.
class renderer_system : public core::system {
	struct per_frame_data_t {
		vk::UniqueCommandBuffer primary_command_buffer;
		graphics::simple_uniform_buffer uniform_buffer;
		graphics::simple_descriptor_pool discriptor_pool;
		boost::optional<graphics::descriptor_set> scene_descriptor_set;
	};
	struct per_thread_data_t {
		graphics::command_pool command_pool;
	};
	struct per_frame_per_thread_data_t {
		vk::UniqueCommandBuffer command_buffer;
		graphics::growing_simple_descriptor_pool discriptor_pool;
	};

	core::engine& eng_;
	graphics::graphics_system& gs_;
	renderer_system_settings settings_;
	rendering::model_manager mdl_mgr;
	rendering::material_manager mat_mgr;
	graphics::command_pool primary_cmd_pool;

	std::shared_ptr<const graphics::sampler> default_sampler_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_scene_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_material_;
	std::shared_ptr<const graphics::pipeline_layout> pipeline_layout_scene_pass_;
	std::shared_ptr<const graphics::render_pass> main_render_pass_;
	std::unique_ptr<graphics::framebuffer> main_framebuffer_;
	std::shared_ptr<const graphics::pipeline> main_forward_pipeline_;

	containers::dynamic_array<per_frame_data_t> per_frame_data_;
	std::unique_ptr<containers::per_thread<per_thread_data_t>> per_thread_data_;
	containers::dynamic_array<containers::per_thread<per_frame_per_thread_data_t>> per_frame_per_thread_data_;

	std::vector<vk::CommandBuffer> secondary_cmdbuff_handles_tmp;

	friend class static_model_component;
	friend class technique;

	void create_samplers();
	void create_descriptor_sets();
	void create_pipeline_layouts();
	void create_render_passes_and_framebuffers();
	void create_pipelines();
	void create_per_frame_data();
	void create_per_thread_data();
	void create_per_frame_per_thread_data();

	friend class renderer_state;

	per_frame_data_t& per_frame_data();
	per_frame_per_thread_data_t& per_frame_per_thread_data();
	per_thread_data_t& per_thread_data();

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

	/// \brief Provides access to a sampler object that can be used for regular texture sampling without
	/// special requirements.
	const graphics::sampler& default_sampler() const {
		assert(default_sampler_);
		return *default_sampler_;
	}

	/// Provides access to the descriptor_set_layout for the descriptor_set providing per-material resources.
	const std::shared_ptr<const graphics::descriptor_set_layout>& descriptor_set_layout_per_material() const {
		return descriptor_set_layout_per_material_;
	}

	/// Provides access to the descriptor_set_layout for the descriptor_set providing per-scene resources.
	const std::shared_ptr<const graphics::descriptor_set_layout>& descriptor_set_layout_per_scene() const {
		return descriptor_set_layout_per_scene_;
	}

	/// Provides access to the graphics::pipeline used for the main forward rendering pass.
	const graphics::pipeline& main_forward_pipeline() const {
		assert(main_forward_pipeline_);
		return *main_forward_pipeline_;
	}

	/// Provides access to the main framebuffer object.
	graphics::framebuffer& main_framebuffer() const {
		assert(main_framebuffer_);
		return *main_framebuffer_;
	}

	/// Provides access to the render_pass object used for the main forward rendering pass.
	const graphics::render_pass& main_render_pass() const {
		assert(main_render_pass_);
		return *main_render_pass_;
	}

	/// \brief Provides access to the pipeline_layout used for pipeline in scene passes (drawing objects in
	/// the scene).
	const graphics::pipeline_layout& pipeline_layout_scene_pass() const {
		assert(pipeline_layout_scene_pass_);
		return *pipeline_layout_scene_pass_;
	}

	/// Allows read-only access to the settings specified on construction.
	const renderer_system_settings& settings() const {
		return settings_;
	}

	/// Allows access to the model_manager.
	const rendering::model_manager& model_manager() const {
		return mdl_mgr;
	}

	/// Allows access to the model_manager.
	rendering::model_manager& model_manager() {
		return mdl_mgr;
	}

	/// Allows access to the material_manager.
	const rendering::material_manager& material_manager() const {
		return mat_mgr;
	}

	/// Allows access to the material_manager.
	rendering::material_manager& material_manager() {
		return mat_mgr;
	}
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
