/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_SYSTEM_HPP_
#define MCE_RENDERING_RENDERER_SYSTEM_HPP_

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
} // namespace graphics
namespace rendering {

class renderer_system : public core::system {
	graphics::graphics_system& gs_;
	model_manager mdl_mgr;
	material_manager mat_mgr;

	std::shared_ptr<const graphics::sampler> default_sampler_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_scene_;
	std::shared_ptr<const graphics::descriptor_set_layout> descriptor_set_layout_per_material_;
	std::shared_ptr<const graphics::pipeline_layout> pipeline_layout_forward_opaque_;

	friend class static_model_component;

public:
	int pre_phase_ordering() const noexcept override {
		return 0x1200;
	}
	int post_phase_ordering() const noexcept override {
		return 0x1200;
	}

	renderer_system(core::engine& eng, graphics::graphics_system& gs);
	~renderer_system();

	void prerender(const mce::core::frame_time& frame_time) override;
	void postrender(const mce::core::frame_time& frame_time) override;
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
