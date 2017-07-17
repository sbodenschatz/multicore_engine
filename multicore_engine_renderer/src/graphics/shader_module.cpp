/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/shader_module.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset.hpp>
#include <mce/asset/asset_manager.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/shader_module.hpp>
#include <stdexcept>

namespace mce {
namespace graphics {

shader_module::shader_module(device& dev, asset::asset_manager& asset_mgr, const std::string& asset_name) {
	auto binary_asset = asset_mgr.load_asset_sync(asset_name);
	if(!binary_asset) std::runtime_error("Couldn't load shader binary asset.");
	load_shader_module(dev, *binary_asset);
}

shader_module::shader_module(device& dev, const asset::asset& shader_binary_asset) {
	load_shader_module(dev, shader_binary_asset);
}

shader_module::~shader_module() {}

void shader_module::load_shader_module(device& dev, const asset::asset& shader_binary_asset) {
	if(!shader_binary_asset.ready())
		throw std::runtime_error("Shader binary asset not ready yet, deferred loading not supported in "
								 "shader_module constructor.");
	vk::Device owner_dev = dev.native_device();
	vk::ShaderModuleCreateInfo ci;
	ci.codeSize = shader_binary_asset.size();
	// TODO: Check if alignment has to be ensured here.
	ci.pCode = reinterpret_cast<const uint32_t*>(shader_binary_asset.data());
	native_shader_module_ = owner_dev.createShaderModuleUnique(ci);
}

} /* namespace graphics */
} /* namespace mce */
