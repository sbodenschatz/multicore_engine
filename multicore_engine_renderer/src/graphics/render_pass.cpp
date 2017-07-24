/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/render_pass.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/graphics/device.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace graphics {

render_pass::render_pass(device& device_, std::shared_ptr<framebuffer_layout> fb_layout,
						 vk::ArrayProxy<attachment_access> attachment_access_modes)
		: device_(device_), fb_layout_{std::move(fb_layout)} {

	UNUSED(attachment_access_modes);
	// TODO: Implement
}

render_pass::~render_pass() {}

} /* namespace graphics */
} /* namespace mce */
