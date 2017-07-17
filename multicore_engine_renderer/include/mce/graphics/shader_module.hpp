/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/shader_module.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_SHADER_MODULE_HPP_
#define GRAPHICS_SHADER_MODULE_HPP_

#include <vulkan/vulkan.hpp>

namespace mce {
namespace asset {
class asset;
class asset_manager;
} // namespace asset

namespace graphics {
class device;

class shader_module {
private:
	vk::UniqueShaderModule native_shader_module_;
	std::string name;

	void load_shader_module(device& dev, const asset::asset& shader_binary_asset);

public:
	shader_module(device& dev, asset::asset_manager& asset_mgr, const std::string& asset_name);
	shader_module(device& dev, const asset::asset& shader_binary_asset);
	~shader_module();

	vk::ShaderModule native_shader_module() const {
		return native_shader_module_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_SHADER_MODULE_HPP_ */
