/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/shader_loader.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SHADER_LOADER_HPP_
#define MCE_GRAPHICS_SHADER_LOADER_HPP_

#include <boost/container/flat_map.hpp>
#include <condition_variable>
#include <mutex>
#include <string>

namespace mce {
namespace asset {
class asset_manager;
} // namespace asset
namespace graphics {
class shader_module;
class graphics_manager;

class shader_loader {
	asset::asset_manager& amgr;
	graphics_manager& gmgr;
	size_t pending_loads;
	mutable std::mutex shaders_mtx;
	mutable std::condition_variable shaders_cv;
	boost::container::flat_map<std::string, std::shared_ptr<const shader_module>> shaders;

public:
	shader_loader(asset::asset_manager& amgr, graphics_manager& gmgr);
	~shader_loader();

	void load_shader(const std::string& name);
	std::shared_ptr<const shader_module> get_shader(const std::string& name) const;
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SHADER_LOADER_HPP_ */
