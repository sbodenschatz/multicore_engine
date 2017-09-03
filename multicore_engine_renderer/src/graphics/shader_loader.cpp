/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/shader_loader.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/shader_loader.hpp>

namespace mce {
namespace graphics {

shader_loader::shader_loader(asset::asset_manager& amgr, graphics_manager& gmgr)
		: amgr{amgr}, gmgr{gmgr}, pending_loads{0} {
	// TODO Auto-generated constructor stub
}

shader_loader::~shader_loader() {}

} /* namespace graphics */
} /* namespace mce */
