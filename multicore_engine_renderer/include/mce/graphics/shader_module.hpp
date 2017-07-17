/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/shader_module.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_SHADER_MODULE_HPP_
#define GRAPHICS_SHADER_MODULE_HPP_

/**
 * \file
 * Provides a shader_module class that encapsulates the handling of vulkan shader modules.
 */

#include <vulkan/vulkan.hpp>

namespace mce {
namespace asset {
class asset;
class asset_manager;
} // namespace asset

namespace graphics {
class device;

/// Represents a shader module loaded into vulkan.
/**
 * These shader modules are used in the creation of vulkan pipelines.
 *
 * The modules encapsulate a SPIR-V binary containing shader code.
 */
class shader_module {
private:
	vk::UniqueShaderModule native_shader_module_;
	std::string name;

	void load_shader_module(device& dev, const asset::asset& shader_binary_asset);

public:
	/// Creates a shader_module for the given device by loading the named asset from the given asset_manager.
	shader_module(device& dev, asset::asset_manager& asset_mgr, const std::string& asset_name);
	/// Creates a shader_module for the given device from the given asset containing the shader binary.
	shader_module(device& dev, const asset::asset& shader_binary_asset);
	/// Destroys the shader_module and releases the underlying resources.
	~shader_module();

	/// Allows access to the underlying native vulkan shader module object.
	vk::ShaderModule native_shader_module() const {
		return native_shader_module_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_SHADER_MODULE_HPP_ */
