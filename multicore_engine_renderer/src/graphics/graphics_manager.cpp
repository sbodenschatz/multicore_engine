/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/graphics_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

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

} /* namespace graphics */
} /* namespace mce */
