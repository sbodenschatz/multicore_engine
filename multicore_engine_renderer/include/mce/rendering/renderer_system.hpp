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
} // namespace graphics
namespace rendering {

class renderer_system : public core::system {

	model_manager mdl_mgr;
	material_manager mat_mgr;

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
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_SYSTEM_HPP_ */
