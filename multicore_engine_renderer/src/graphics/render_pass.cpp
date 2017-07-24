/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/render_pass.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {

render_pass::render_pass(device& device_, std::shared_ptr<framebuffer_layout> fb_layout)
		: device_(device_), fb_layout_{std::move(fb_layout)} {

	// TODO: Implement
}

render_pass::~render_pass() {}

} /* namespace graphics */
} /* namespace mce */
