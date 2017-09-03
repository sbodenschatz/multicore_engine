/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/technique.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_TECHNIQUE_HPP_
#define MCE_RENDERING_TECHNIQUE_HPP_

#include <mce/graphics/framebuffer_config.hpp>
#include <string>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace graphics {
class window;
} // namespace graphics
namespace rendering {
class renderer_system;

class technique {
	core::engine& eng;
	renderer_system& sys;
	std::string name_;
	std::shared_ptr<const graphics::framebuffer_config> framebuffer_config_;

public:
	technique(core::engine& eng, renderer_system& sys, std::string name, graphics::window& swapchain_window,
			  std::vector<graphics::framebuffer_attachment_config> attachment_configs,
			  std::vector<graphics::framebuffer_pass_config> passes);
	~technique();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_TECHNIQUE_HPP_ */
