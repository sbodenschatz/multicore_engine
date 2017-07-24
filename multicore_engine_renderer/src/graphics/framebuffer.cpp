/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/framebuffer.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/framebuffer.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

framebuffer::framebuffer(device& dev, window& win, const framebuffer_layout& layout)
		: dev_{&dev}, win_{&win}, size_{win.glfw_window().framebuffer_size()} {
	UNUSED(layout);
}

framebuffer::~framebuffer() {}

} /* namespace graphics */
} /* namespace mce */
