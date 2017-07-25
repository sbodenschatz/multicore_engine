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

render_pass::render_pass(device& device_, destruction_queue_manager dqm,
						 std::shared_ptr<subpass_graph> subpasses,
						 std::shared_ptr<framebuffer_layout> fb_layout,
						 vk::ArrayProxy<attachment_access> attachment_access_modes,
						 vk::ArrayProxy<attachment_reference_layout> attachment_ref_layout_map)
		: device_(device_), subpasses_{std::move(subpasses)}, fb_layout_{std::move(fb_layout)} {

	UNUSED(attachment_access_modes);
	UNUSED(attachment_ref_layout_map);
	// TODO: Implement
}

render_pass::~render_pass() {}

} /* namespace graphics */
} /* namespace mce */
