/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/graphics_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_
#define MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_

#include <boost/container/flat_map.hpp>
#include <memory>
#include <mutex>
#include <string>

namespace mce {
namespace graphics {
class shader_module;
class device;
class pipeline_layout;
class pipeline;
class framebuffer_config;
class subpass_graph;
class render_pass;
class pipeline_cache;
class pipeline_config;
class descriptor_set_layout;
class sampler;

class graphics_manager {
private:
	mutable std::mutex manager_mutex_;
	device* dev_;
	std::unique_ptr<pipeline_cache> pipeline_cache_;
	boost::container::flat_map<std::string, std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts_;
	boost::container::flat_map<std::string, std::shared_ptr<pipeline_layout>> pipeline_layouts_;
	boost::container::flat_map<std::string, std::shared_ptr<shader_module>> shader_modules_;
	boost::container::flat_map<std::string, std::shared_ptr<pipeline_config>> pipelines_configs_;
	boost::container::flat_map<std::string, std::shared_ptr<framebuffer_config>> framebuffer_configs_;
	boost::container::flat_map<std::string, std::shared_ptr<subpass_graph>> subpass_graphs_;
	boost::container::flat_map<std::string, std::shared_ptr<render_pass>> render_passes_;
	boost::container::flat_map<std::string, std::shared_ptr<pipeline>> pipelines_;
	boost::container::flat_map<std::string, std::shared_ptr<sampler>> samplers_;

public:
	graphics_manager(device& dev);
	~graphics_manager();

	std::shared_ptr<descriptor_set_layout> find_descriptor_set_layout(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = descriptor_set_layouts_.find(name);
		if(it != descriptor_set_layouts_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<framebuffer_config> find_framebuffer_config(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = framebuffer_configs_.find(name);
		if(it != framebuffer_configs_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<pipeline_layout> find_pipeline_layout(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipeline_layouts_.find(name);
		if(it != pipeline_layouts_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<pipeline> find_pipeline(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipelines_.find(name);
		if(it != pipelines_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<pipeline_config> find_pipelines_config(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipelines_configs_.find(name);
		if(it != pipelines_configs_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<render_pass> find_render_passe(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = render_passes_.find(name);
		if(it != render_passes_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<sampler> find_sampler(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = samplers_.find(name);
		if(it != samplers_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<shader_module> find_shader_module(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = shader_modules_.find(name);
		if(it != shader_modules_.end())
			return it->second;
		else
			return {};
	}

	std::shared_ptr<subpass_graph> find_subpass_graph(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = subpass_graphs_.find(name);
		if(it != subpass_graphs_.end())
			return it->second;
		else
			return {};
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_ */
