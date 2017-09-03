/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/shader_loader.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/shader_loader.hpp>

namespace mce {
namespace graphics {

shader_loader::shader_loader(asset::asset_manager& amgr, graphics_manager& gmgr)
		: amgr{amgr}, gmgr{gmgr}, pending_loads{0} {}

shader_loader::~shader_loader() {
	std::unique_lock<std::mutex> lock(shaders_mtx);
	shaders_cv.wait(lock, [this]() { return pending_loads == 0; });
}

} /* namespace graphics */
} /* namespace mce */
