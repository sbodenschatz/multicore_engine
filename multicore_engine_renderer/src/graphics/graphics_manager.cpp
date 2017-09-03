/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
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
#include <mce/graphics/window.hpp>
#include <mce/util/finally.hpp>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

namespace mce {
namespace graphics {

graphics_manager::graphics_manager(device& dev, destruction_queue_manager* dqm)
		: dev_{&dev}, dqm_{dqm}, pipeline_cache_{std::make_unique<pipeline_cache>(dev)} {}

graphics_manager::~graphics_manager() {}

std::shared_ptr<const descriptor_set_layout>
graphics_manager::create_descriptor_set_layout(const std::string& name,
											   // cppcheck-suppress passedByValue
											   std::vector<descriptor_set_layout_binding_element> bindings) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = descriptor_set_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<descriptor_set_layout>(*dev_, dqm_, std::move(bindings));
	return entry;
}

std::shared_ptr<const framebuffer_config>
graphics_manager::create_framebuffer_config(const std::string& name,
											// cppcheck-suppress passedByValue
											std::vector<framebuffer_attachment_config> attachment_configs,
											// cppcheck-suppress passedByValue
											std::vector<framebuffer_pass_config> passes) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = framebuffer_configs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<framebuffer_config>(std::move(attachment_configs), std::move(passes));
	return entry;
}

std::shared_ptr<const framebuffer_config>
graphics_manager::create_framebuffer_config(const std::string& name, window& swapchain_window,
											// cppcheck-suppress passedByValue
											std::vector<framebuffer_attachment_config> attachment_configs,
											// cppcheck-suppress passedByValue
											std::vector<framebuffer_pass_config> passes) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = framebuffer_configs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<framebuffer_config>(
			swapchain_window.make_framebuffer_config(std::move(attachment_configs), std::move(passes)));
	return entry;
}

std::shared_ptr<const pipeline_layout> graphics_manager::create_pipeline_layout(
		const std::string& name,
		// cppcheck-suppress passedByValue
		std::vector<std::shared_ptr<const descriptor_set_layout>> descriptor_set_layouts,
		// cppcheck-suppress passedByValue
		std::vector<vk::PushConstantRange> push_constant_ranges) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = pipeline_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<pipeline_layout>(*dev_, dqm_, std::move(descriptor_set_layouts),
											  std::move(push_constant_ranges));
	return entry;
}
std::shared_ptr<const pipeline_layout> graphics_manager::create_pipeline_layout_by_names(
		const std::string& name, vk::ArrayProxy<const std::string> descriptor_set_layout_names,
		// cppcheck-suppress passedByValue
		std::vector<vk::PushConstantRange> push_constant_ranges) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = pipeline_layouts_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	std::vector<std::shared_ptr<const descriptor_set_layout>> layouts;
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
std::shared_ptr<const render_pass> graphics_manager::create_render_pass(
		// cppcheck-suppress passedByValue
		const std::string& name, std::shared_ptr<const subpass_graph> subpasses,
		// cppcheck-suppress passedByValue
		std::shared_ptr<const framebuffer_config> fb_config, uint32_t fb_pass_config,
		vk::ArrayProxy<const render_pass_attachment_access> attachment_access_modes) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = render_passes_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<render_pass>(*dev_, dqm_, std::move(subpasses), std::move(fb_config),
										  fb_pass_config, attachment_access_modes);
	return entry;
}
std::shared_ptr<const render_pass> graphics_manager::create_render_pass(
		const std::string& name, const std::string& subpass_graph_name, const std::string& fb_config_name,
		uint32_t fb_pass_config,
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
	entry = std::make_shared<render_pass>(*dev_, dqm_, subpasses_it->second, fbcfg_it->second, fb_pass_config,
										  attachment_access_modes);
	return entry;
}
std::shared_ptr<const subpass_graph>
		// cppcheck-suppress passedByValue
		graphics_manager::create_subpass_graph(const std::string& name, std::vector<subpass_entry> subpasses,
											   // cppcheck-suppress passedByValue
											   std::vector<vk::SubpassDependency> dependencies) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto& entry = subpass_graphs_[name];
	if(entry) throw mce::key_already_used_exception("The given name is already in use.");
	entry = std::make_shared<subpass_graph>(std::move(subpasses), std::move(dependencies));
	return entry;
}
std::shared_ptr<const shader_module>
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

std::shared_ptr<const sampler>
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
void graphics_manager::add_pending_pipeline(const std::string& name,
											// cppcheck-suppress passedByValue
											std::shared_ptr<const pipeline_config> cfg) {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	auto it = pipelines_.find(name);
	auto it_cfg = pipeline_configs_.find(name);
	auto it_pending = std::find_if(pending_pipeline_configs_.begin(), pending_pipeline_configs_.end(),
								   [&name](const auto& v) { return v.name == name; });
	if(it != pipelines_.end() || it_cfg != pipeline_configs_.end() ||
	   it_pending != pending_pipeline_configs_.end()) {
		throw mce::key_already_used_exception("The given name is already in use.");
	}
	pending_pipeline_configs_.push_back(pending_pipeline_task{name, std::move(cfg), nullptr});
}

void graphics_manager::compile_pending_pipelines() {
	std::lock_guard<std::mutex> lock(manager_mutex_);
	static std::vector<pending_pipeline_task> processing_pipeline_configs;
	auto clear_processing = util::finally([&]() { processing_pipeline_configs.clear(); });
	using std::swap;
	swap(processing_pipeline_configs, pending_pipeline_configs_);
	using range = tbb::blocked_range<decltype(processing_pipeline_configs.begin())>;
	tbb::parallel_for(range(processing_pipeline_configs.begin(), processing_pipeline_configs.end()),
					  [this](range tasks) {
						  if(tasks.empty()) {
							  return;
						  } else if(tasks.size() == 1) {
							  tasks.begin()->result = std::make_shared<pipeline>(pipeline::create_pipeline(
									  *dev_, dqm_, *pipeline_cache_, *(tasks.begin()->config)));
						  } else {
							  std::vector<pipeline_config> configs;
							  configs.reserve(tasks.size());
							  std::transform(tasks.begin(), tasks.end(), std::back_inserter(configs),
											 [](const pending_pipeline_task& task) { return *task.config; });
							  auto res = pipeline::create_pipelines(*dev_, dqm_, *pipeline_cache_,
																	std::move(configs));
							  for(size_t i = 0; i < tasks.size(); ++i) {
								  tasks.begin()[i].result = std::make_shared<pipeline>(std::move(res[i]));
							  }
						  }
					  },
					  tbb::auto_partitioner());
	for(auto& task : processing_pipeline_configs) {
		pipeline_configs_[task.name] = std::move(task.config);
		pipelines_[task.name] = std::move(task.result);
	}
}

} /* namespace graphics */
} /* namespace mce */
