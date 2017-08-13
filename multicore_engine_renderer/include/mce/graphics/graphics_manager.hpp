/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/graphics_manager.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_
#define MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_

/**
 * Defines the graphics_manager class.
 */

#include <boost/container/flat_map.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace asset {
class asset;
} // namespace asset

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
class destruction_queue_manager;
class framebuffer_attachment_config;
struct descriptor_set_layout_binding_element;
class sampler_addressing_mode;
struct subpass_entry;
struct render_pass_attachment_access;
class window;

/// Manages global configuration state objects for the graphics subsystem.
/**
 * The managed types of objects are:
 * - Descriptor set layouts
 * - Pipeline layouts
 * - Shader modules
 * - Pipelines and pipeline configs
 * - Framebuffer configs
 * - Subpass graphs
 * - Render passes
 * - Samplers
 *
 * These objects are managed by unique names per type and are held in shared ownership.
 * User code that retrieves objects from the manager participate in ownership.
 * Therefore objects can be released from a name without worrying about invalidation for users as long as the
 * users hold on to the shared pointer.
 *
 * Additionally holds a pipeline_cache for compiling pipelines.
 *
 * The member functions of this class are implemented in a thread-safe manner.
 */
class graphics_manager {
private:
	struct pending_pipeline_task {
		std::string name;
		std::shared_ptr<const pipeline_config> config;
		std::shared_ptr<const pipeline> result;
	};

	mutable std::mutex manager_mutex_;
	device* dev_;
	destruction_queue_manager* dqm_;
	std::unique_ptr<pipeline_cache> pipeline_cache_;
	boost::container::flat_map<std::string, std::shared_ptr<const descriptor_set_layout>>
			descriptor_set_layouts_;
	boost::container::flat_map<std::string, std::shared_ptr<const pipeline_layout>> pipeline_layouts_;
	boost::container::flat_map<std::string, std::shared_ptr<const shader_module>> shader_modules_;
	std::vector<pending_pipeline_task> pending_pipeline_configs_;
	boost::container::flat_map<std::string, std::shared_ptr<const pipeline_config>> pipeline_configs_;
	boost::container::flat_map<std::string, std::shared_ptr<const framebuffer_config>> framebuffer_configs_;
	boost::container::flat_map<std::string, std::shared_ptr<const subpass_graph>> subpass_graphs_;
	boost::container::flat_map<std::string, std::shared_ptr<const render_pass>> render_passes_;
	boost::container::flat_map<std::string, std::shared_ptr<const pipeline>> pipelines_;
	boost::container::flat_map<std::string, std::shared_ptr<const sampler>> samplers_;

public:
	/// \brief Creates a graphics_manager for the given device and using the given destruction_queue_manager
	/// for the managed resources.
	graphics_manager(device& dev, destruction_queue_manager* dqm);
	/// Releases ownership of all registered resources, destroying now unused objects.
	~graphics_manager();

	/// Forbids copying.
	graphics_manager(const graphics_manager&) = delete;
	/// Forbids copying.
	graphics_manager& operator=(const graphics_manager&) = delete;
	/// Forbids moving.
	graphics_manager(graphics_manager&&) = delete;
	/// Forbids moving.
	graphics_manager& operator=(graphics_manager&&) = delete;

	/// \brief Adds a task to create a pipeline from the given pipeline_config and register the pipeline and
	/// the pipeline_config under the given name.
	/**
	 * Only stores the task. The actual compilation of all pending tasks is done in parallel in
	 * compile_pending_pipelines().
	 */
	void add_pending_pipeline(const std::string& name, std::shared_ptr<const pipeline_config> cfg);
	/// Compiles all pipelines that were prepared using add_pending_pipeline().
	/**
	 * The compilation is done in parallel over the available hardware threads.
	 * After completion the pipelines are made available under the names that were specified when they were
	 * added to the pending list.
	 */
	void compile_pending_pipelines();

	/// Creates a descriptor_set_layout with the given bindings and stores it under the given name.
	std::shared_ptr<const descriptor_set_layout>
	create_descriptor_set_layout(const std::string& name,
								 std::vector<descriptor_set_layout_binding_element> bindings);

	/// \brief Creates a framebuffer_config containing the given attachment configurations and stores it under
	/// the given name.
	std::shared_ptr<const framebuffer_config>
	create_framebuffer_config(const std::string& name,
							  std::vector<framebuffer_attachment_config> attachment_configs);

	/// \brief Creates a framebuffer_config containing a config for a swapchain attachment from the given
	/// window and the given attachment configurations and stores it under the given name.
	std::shared_ptr<const framebuffer_config>
	create_framebuffer_config(const std::string& name, window& swapchain_window,
							  std::vector<framebuffer_attachment_config> attachment_configs);

	/// \brief Creates a pipeline_layout consisting of the given descriptor_set_layout objects and using the
	/// given push_constant_ranges and stores it under the given name.
	std::shared_ptr<const pipeline_layout>
	create_pipeline_layout(const std::string& name,
						   std::vector<std::shared_ptr<const descriptor_set_layout>> descriptor_set_layouts,
						   std::vector<vk::PushConstantRange> push_constant_ranges = {});
	/// \brief Creates a pipeline_layout consisting of the given descriptor_set_layout objects with the given
	/// names and using the given push_constant_ranges and stores it under the given name.
	/**
	 * Avoids the overhead of separately retrieving each descriptor_set_layout taking a lock for each access.
	 */
	std::shared_ptr<const pipeline_layout>
	create_pipeline_layout(const std::string& name,
						   vk::ArrayProxy<const std::string> descriptor_set_layout_names,
						   std::vector<vk::PushConstantRange> push_constant_ranges = {});

	/// Creates a sampler object with the given parameters and stores it under the given name.
	std::shared_ptr<const sampler> create_sampler(const std::string& name, vk::Filter mag_filter,
												  vk::Filter min_filter, vk::SamplerMipmapMode mipmap_mode,
												  sampler_addressing_mode address_mode, float mip_lod_bias,
												  boost::optional<float> max_anisotropy,
												  boost::optional<vk::CompareOp> compare_op, float min_lod,
												  float max_lod, vk::BorderColor border_color,
												  bool unnormalized_coordinates);

	/// \brief Creates a subpass_graph from the given subpass configurations and dependencies between the
	/// subpasses and stores it under the given name.
	std::shared_ptr<const subpass_graph>
	create_subpass_graph(const std::string& name, std::vector<subpass_entry> subpasses,
						 std::vector<vk::SubpassDependency> dependencies);

	/// \brief Creates a shader_module from the given asset containing a SPIR-V binary and stores it under the
	/// given name.
	/**
	 * Requires that the asset is ready, i.e. has finished loading.
	 */
	std::shared_ptr<const shader_module> create_shader_module(const std::string& name,
															  const asset::asset& ready_shader_binary_asset);

	/// \brief Creates a render_pass using the given subpass_graph, framebuffer_config and attachment access
	/// modes and stores it under the given name.
	std::shared_ptr<const render_pass>
	create_render_pass(const std::string& name, std::shared_ptr<const subpass_graph> subpasses,
					   std::shared_ptr<const framebuffer_config> fb_config,
					   vk::ArrayProxy<const render_pass_attachment_access> attachment_access_modes);
	/// \brief Creates a render_pass using the subpass_graph and framebuffer_config with the given names and
	/// the given attachment access modes and stores it under the given name.
	/**
	 * Avoids the overhead of separately retrieving the subpass_graph and framebuffer_config up-front, taking
	 * a lock for each access.
	 */
	std::shared_ptr<const render_pass>
	create_render_pass(const std::string& name, const std::string& subpass_graph_name,
					   const std::string& fb_config_name,
					   vk::ArrayProxy<const render_pass_attachment_access> attachment_access_modes);

	/// Releases ownership of the descriptor_set_layout object with the given name.
	void release_descriptor_set_layout(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		descriptor_set_layouts_.erase(name);
	}
	/// Releases ownership of the framebuffer_config object with the given name.
	void release_framebuffer_config(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		framebuffer_configs_.erase(name);
	}
	/// Releases ownership of the pipeline_layout with the given name.
	void release_pipeline_layout(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		pipeline_layouts_.erase(name);
	}
	/// Releases ownership of the sampler object with the given name.
	void release_sampler(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		samplers_.erase(name);
	}
	/// Releases ownership of the subpass_graph object with the given name.
	void release_subpass_graph(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		subpass_graphs_.erase(name);
	}
	/// Releases ownership of the shader_module with the given name.
	void release_shader_module(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		shader_modules_.erase(name);
	}
	/// Releases ownership of the pipeline and pipeline_config with the given name.
	void release_pipeline_and_config(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		pipeline_configs_.erase(name);
		pipelines_.erase(name);
	}
	/// Releases ownership of the render_pass with the given name.
	void release_render_pass(const std::string& name) {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		render_passes_.erase(name);
	}

	/// Returns the descriptor_set_layout with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const descriptor_set_layout> find_descriptor_set_layout(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = descriptor_set_layouts_.find(name);
		if(it != descriptor_set_layouts_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the framebuffer_config with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const framebuffer_config> find_framebuffer_config(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = framebuffer_configs_.find(name);
		if(it != framebuffer_configs_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the pipeline_layout with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const pipeline_layout> find_pipeline_layout(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipeline_layouts_.find(name);
		if(it != pipeline_layouts_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the pipeline object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const pipeline> find_pipeline(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipelines_.find(name);
		if(it != pipelines_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the pipeline_config object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const pipeline_config> find_pipeline_config(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = pipeline_configs_.find(name);
		if(it != pipeline_configs_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the render_pass object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const render_pass> find_render_pass(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = render_passes_.find(name);
		if(it != render_passes_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the sampler object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const sampler> find_sampler(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = samplers_.find(name);
		if(it != samplers_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the shader_module object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const shader_module> find_shader_module(const std::string& name) const {
		std::lock_guard<std::mutex> lock(manager_mutex_);
		auto it = shader_modules_.find(name);
		if(it != shader_modules_.end())
			return it->second;
		else
			return {};
	}

	/// Returns the subpass_graph object with the given name or an empty shared_ptr if it doesn't exist.
	std::shared_ptr<const subpass_graph> find_subpass_graph(const std::string& name) const {
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
