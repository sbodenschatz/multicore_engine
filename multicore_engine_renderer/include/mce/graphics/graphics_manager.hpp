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
	std::mutex manager_mutex_;
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
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_GRAPHICS_MANAGER_HPP_ */
