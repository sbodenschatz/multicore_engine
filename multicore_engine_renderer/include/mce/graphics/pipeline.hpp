/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/pipeline.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_HPP_
#define GRAPHICS_PIPELINE_HPP_

#include <mce/graphics/pipeline_config.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;
class pipeline_cache;

class pipeline {
private:
	vk::UniquePipeline native_pipeline;

	pipeline();

public:
	pipeline(pipeline&&) = default;
	pipeline& operator=(pipeline&&) = default;
	~pipeline();
	static std::vector<pipeline> create_pipelines(const device& dev, pipeline_cache& pipeline_cache,
												  const std::vector<pipeline_config>& pipeline_configs);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_HPP_ */
