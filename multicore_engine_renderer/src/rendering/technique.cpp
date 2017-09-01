/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/technique.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/rendering/renderer_system.hpp>
#include <mce/rendering/technique.hpp>

namespace mce {
namespace rendering {

// cppcheck-suppress passedByValue
technique::technique(core::engine& eng, renderer_system& sys, std::string name,
					 graphics::window& swapchain_window,
					 // cppcheck-suppress passedByValue
					 std::vector<graphics::framebuffer_attachment_config> attachment_configs)
		: eng{eng}, sys{sys}, name_{std::move(name)} {
	framebuffer_config_ = sys.gs_.graphics_manager().create_framebuffer_config(
			name_ + "_fbcfg", swapchain_window, std::move(attachment_configs));
}

technique::~technique() {}

} /* namespace rendering */
} /* namespace mce */
