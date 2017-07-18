/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/pipeline.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline.hpp>
#include <mce/graphics/pipeline_cache.hpp>

namespace mce {
namespace graphics {

pipeline::pipeline() {}

pipeline::~pipeline() {}

std::vector<pipeline> pipeline::create_pipelines(const device& dev, pipeline_cache& pipeline_cache,
												 const std::vector<pipeline_config>& pipeline_configs) {
	std::vector<pipeline> pipelines;
	vk::Device owner_dev = dev.native_device();
	std::vector<vk::GraphicsPipelineCreateInfo> pipelines_ci;
	std::transform(pipeline_configs.begin(), pipeline_configs.end(), std::back_inserter(pipelines_ci),
				   [&, owner_dev](pipeline_config pcfg) { return pcfg.generate_create_info_structure(); });
	auto native_pipelines =
			owner_dev.createGraphicsPipelinesUnique(pipeline_cache.native_pipeline_cache(), pipelines_ci);
	std::transform(native_pipelines.begin(), native_pipelines.end(), std::back_inserter(pipelines),
				   [owner_dev](vk::UniquePipeline& native_pipeline) {
					   pipeline p;
					   p.native_pipeline = std::move(native_pipeline);
					   return p;
				   });
	return pipelines;
}

} /* namespace graphics */
} /* namespace mce */
