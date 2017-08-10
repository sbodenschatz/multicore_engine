/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset.hpp>
#include <mce/exceptions.hpp>
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
#include <mce/graphics/shader_module.hpp>

namespace mce {
namespace graphics {

graphics_manager::graphics_manager(device& dev, destruction_queue_manager* dqm) : dev_{&dev}, dqm_{dqm} {
	// TODO Auto-generated constructor stub
}

graphics_manager::~graphics_manager() {}

std::shared_ptr<descriptor_set_layout>
graphics_manager::create_descriptor_set_layout(const std::string& name,
											   std::vector<descriptor_set_layout_binding_element> bindings) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = descriptor_set_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<descriptor_set_layout>(*dev_, dqm_, std::move(bindings));
	return entry;
}

std::shared_ptr<framebuffer_config> graphics_manager::create_framebuffer_config(
		const std::string& name, vk::ArrayProxy<const framebuffer_attachment_config> attachment_configs) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = framebuffer_configs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<framebuffer_config>(attachment_configs);
	return entry;
}
std::shared_ptr<framebuffer_config>
graphics_manager::create_framebuffer_config(const std::string& name,
											std::vector<framebuffer_attachment_config>&& attachment_configs) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = framebuffer_configs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<framebuffer_config>(std::move(attachment_configs));
	return entry;
}

std::shared_ptr<pipeline_layout> graphics_manager::create_pipeline_layout(
		const std::string& name, std::vector<std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts,
		std::vector<vk::PushConstantRange> push_constant_ranges) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = pipeline_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<pipeline_layout>(*dev_, dqm_, std::move(descriptor_set_layouts),
											  std::move(push_constant_ranges));
	return entry;
}
std::shared_ptr<pipeline_layout>
graphics_manager::create_pipeline_layout(const std::string& name,
										 vk::ArrayProxy<const std::string> descriptor_set_layout_names,
										 std::vector<vk::PushConstantRange> push_constant_ranges) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = pipeline_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	std::vector<std::shared_ptr<descriptor_set_layout>> layouts;
	layouts.reserve(descriptor_set_layout_names.size());
	for(const auto& dsl_name : descriptor_set_layout_names) {
		auto it = descriptor_set_layouts_.find(dsl_name);
		if(it != descriptor_set_layouts_.end() && it->second) {
			layouts.push_back(it->second);
		} else {
			throw mce::key_not_found_exception("Descriptor set layout '" + dsl_name + "' not found.");
		}
	}
	entry = std::make_shared<pipeline_layout>(*dev_, dqm_, std::move(layouts),
											  std::move(push_constant_ranges));
	return entry;
}
std::shared_ptr<render_pass> graphics_manager::create_render_pass(
		const std::string& name, std::shared_ptr<subpass_graph> subpasses,
		std::shared_ptr<framebuffer_config> fb_config,
		vk::ArrayProxy<const render_pass_attachment_access> attachment_access_modes) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = render_passes_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<render_pass>(*dev_, dqm_, std::move(subpasses), std::move(fb_config),
										  attachment_access_modes);
	return entry;
}
std::shared_ptr<render_pass> graphics_manager::create_render_pass(
		const std::string& name, const std::string& subpass_graph_name, const std::string& fb_config_name,
		vk::ArrayProxy<const render_pass_attachment_access> attachment_access_modes) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = render_passes_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	auto subpasses_it = subpass_graphs_.find(subpass_graph_name);
	if(subpasses_it == subpass_graphs_.end() || !(subpasses_it->second))
		throw mce::key_not_found_exception("Subpass graph '" + subpass_graph_name + "' not found.");
	auto fbcfg_it = framebuffer_configs_.find(fb_config_name);
	if(fbcfg_it == framebuffer_configs_.end() || !(fbcfg_it->second))
		throw mce::key_not_found_exception("Framebuffer config '" + fb_config_name + "' not found.");
	entry = std::make_shared<render_pass>(*dev_, dqm_, subpasses_it->second, fbcfg_it->second,
										  attachment_access_modes);
	return entry;
}
std::shared_ptr<subpass_graph>
graphics_manager::create_subpass_graph(const std::string& name, std::vector<subpass_entry> subpasses,
									   std::vector<vk::SubpassDependency> dependencies) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = subpass_graphs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<subpass_graph>(std::move(subpasses), std::move(dependencies));
	return entry;
}
std::shared_ptr<shader_module>
graphics_manager::create_shader_module(const std::string& name,
									   const asset::asset& ready_shader_binary_asset) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	if(!ready_shader_binary_asset.ready())
		throw mce::async_state_exception("Given asset '" + ready_shader_binary_asset.name() + "' not ready.");
	auto& entry = shader_modules_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<shader_module>(*dev_, ready_shader_binary_asset);
	return entry;
}

std::shared_ptr<sampler>
graphics_manager::create_sampler(const std::string& name, vk::Filter mag_filter, vk::Filter min_filter,
								 vk::SamplerMipmapMode mipmap_mode, sampler_addressing_mode address_mode,
								 float mip_lod_bias, boost::optional<float> max_anisotropy,
								 boost::optional<vk::CompareOp> compare_op, float min_lod, float max_lod,
								 vk::BorderColor border_color, bool unnormalized_coordinates) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = samplers_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<sampler>(*dev_, dqm_, mag_filter, min_filter, mipmap_mode, address_mode,
									  mip_lod_bias, max_anisotropy, compare_op, min_lod, max_lod,
									  border_color, unnormalized_coordinates);
	return entry;
}

} /* namespace graphics */
} /* namespace mce */
