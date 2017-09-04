/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/shader_loader.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SHADER_LOADER_HPP_
#define MCE_GRAPHICS_SHADER_LOADER_HPP_

/**
 * \file
 * Defines the shader_loader helper class.
 */

#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset
namespace graphics {
class shader_module;
class graphics_manager;

/// \brief Implements functionality to asynchronously load shader assets from an asset::asset_manager and
/// create shader_modules in a graphics_manager for them.
/**
 * The asynchronous loads can happen in parallel and concurrently with other (initialization) code. When the
 * other code reaches a point where the shader modules are needed this class provides functionality to wait
 * for completion of the loads. This can either be done using the wait_for_completion() member function which
 * waits for completion and checks for errors or waiting is done by the destructor, which however can't check
 * for errors because destructors need to be noexcept. If only the destructor is used for synchronization
 * error checking needs to be done by checking for the presence of the requested shader_modules in the
 * graphics_manager after destruction of the shader_loader.
 */
class shader_loader {
	asset::asset_manager& amgr;
	graphics_manager& gmgr;
	size_t pending_loads = 0;
	mutable std::mutex shaders_mtx;
	mutable std::condition_variable shaders_cv;
	std::vector<std::string> failed_assets;

public:
	/// Creates a shader_loader that uses the given asset::asset_manager and graphics_manager for loading.
	shader_loader(asset::asset_manager& amgr, graphics_manager& gmgr);
	/// Waits for completion of pending load task and then destroys the shader_loader.
	~shader_loader() noexcept;

	/// \brief Asynchronously loads the shader with the given name and creates a shader module under that name
	/// in the graphics_manager.
	void load_shader(const std::string& name);
	/// \brief Waits for completion of pending load tasks and checks if errors occured, in which case an
	/// exception is thrown.
	void wait_for_completion();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SHADER_LOADER_HPP_ */
