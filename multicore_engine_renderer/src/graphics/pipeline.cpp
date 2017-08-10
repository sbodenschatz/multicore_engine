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

pipeline::pipeline(destruction_queue_manager* dqm, vk::UniquePipeline native_pipeline,
				   std::shared_ptr<pipeline_layout> layout)
		: native_pipeline_{queued_handle<vk::UniquePipeline>(std::move(native_pipeline), dqm)},
		  layout_{std::move(layout)} {}

pipeline::~pipeline() {}

std::vector<pipeline> pipeline::create_pipelines(const device& dev, destruction_queue_manager* dqm,
												 pipeline_cache& pipeline_cache,
												 std::vector<pipeline_config> pipeline_configs) {
	std::vector<pipeline> pipelines;
	vk::Device owner_dev = dev.native_device();
	std::vector<vk::GraphicsPipelineCreateInfo> pipelines_ci;
	pipelines_ci.reserve(pipeline_configs.size());
	std::transform(pipeline_configs.begin(), pipeline_configs.end(), std::back_inserter(pipelines_ci),
				   [&, owner_dev](pipeline_config& pcfg) { return pcfg.generate_create_info_structure(); });
	auto native_pipelines =
			owner_dev.createGraphicsPipelinesUnique(pipeline_cache.native_pipeline_cache(), pipelines_ci);
	pipelines.reserve(native_pipelines.size());
	std::transform(pipeline_configs.begin(), pipeline_configs.end(), native_pipelines.begin(),
				   std::back_inserter(pipelines),
				   [owner_dev, dqm](const pipeline_config& config, vk::UniquePipeline& native_pipeline) {
					   return pipeline(dqm, std::move(native_pipeline), config.layout());
				   });
	return pipelines;
}

void pipeline::bind(vk::CommandBuffer cb) const {
	cb.bindPipeline(vk::PipelineBindPoint::eGraphics, native_pipeline_.get());
}

} /* namespace graphics */
} /* namespace mce */
