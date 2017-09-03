/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/shader_loader.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset_manager.hpp>
#include <mce/graphics/graphics_manager.hpp>
#include <mce/graphics/shader_loader.hpp>

namespace mce {
namespace graphics {

shader_loader::shader_loader(asset::asset_manager& amgr, graphics_manager& gmgr) : amgr{amgr}, gmgr{gmgr} {}

shader_loader::~shader_loader() {
	std::unique_lock<std::mutex> lock(shaders_mtx);
	shaders_cv.wait(lock, [this]() { return pending_loads == 0; });
}

void shader_loader::load_shader(const std::string& name) {
	amgr.load_asset_async(name + ".spv",
						  [name, this](const asset::asset_ptr& shader_asset) {
							  try {
								  {
									  std::lock_guard<std::mutex> lock(shaders_mtx);
									  gmgr.create_shader_module(name, *shader_asset);
									  --pending_loads;
								  }
								  shaders_cv.notify_one();
							  } catch(...) {
								  {
									  std::lock_guard<std::mutex> lock(shaders_mtx);
									  errors = true;
									  --pending_loads;
								  }
								  shaders_cv.notify_one();
							  }
						  },
						  [this](std::exception_ptr) {
							  {
								  std::lock_guard<std::mutex> lock(shaders_mtx);
								  errors = true;
								  --pending_loads;
							  }
							  shaders_cv.notify_one();
						  });
}

} /* namespace graphics */
} /* namespace mce */
