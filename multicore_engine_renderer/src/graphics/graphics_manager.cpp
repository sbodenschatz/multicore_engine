/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/framebuffer_config.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/pipeline.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/pipeline_config.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/graphics/render_pass.hpp>
#include <mce/graphics/sampler.hpp>

class shader_module;
class device;
class pipeline_layout;
class pipeline;
class framebuffer_config;
class subpass_graph;
class render_pass;
class pipeline_cache;
class pipeline_config;
class descriptor_set_layout;
class sampler;

namespace mce {
namespace graphics {

graphics_manager::graphics_manager(device& dev) : dev_{&dev} {
	// TODO Auto-generated constructor stub
}

graphics_manager::~graphics_manager() {}

} /* namespace graphics */
} /* namespace mce */
