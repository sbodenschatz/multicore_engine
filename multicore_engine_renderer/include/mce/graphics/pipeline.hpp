/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/pipeline.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_HPP_
#define GRAPHICS_PIPELINE_HPP_

/**
 * \file
 * Provides the pipeline encapsulation class.
 */

#include <mce/graphics/pipeline_config.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;
class pipeline_cache;

/// Encapsulates a vulkan graphics pipeline state object.
class pipeline {
private:
	vk::UniquePipeline native_pipeline_;
	std::shared_ptr<vk::UniquePipelineLayout> layout_;

	pipeline();

public:
	/// Allows move-construction.
	pipeline(pipeline&&) = default;
	/// Allows move-assignment.
	pipeline& operator=(pipeline&&) = default;
	/// Destroys the pipeline and releases the associated resources.
	~pipeline();
	/// \brief Creates a pipeline for each of the given pipeline_config on the given device using the given
	/// pipeline_cache for build time improvement.
	static std::vector<pipeline> create_pipelines(const device& dev, pipeline_cache& pipeline_cache,
												  const std::vector<pipeline_config>& pipeline_configs);

	/// Bind this pipeline in the given command buffer.
	void bind(vk::CommandBuffer cb) const;

	/// Allows access to the underlying native vulkan pipeline object.
	vk::Pipeline native_pipeline() const {
		return native_pipeline_.get();
	}

	/// Allows access to the pipeline layout used by the pipeline.
	const std::shared_ptr<vk::UniquePipelineLayout>& layout() const {
		return layout_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_HPP_ */
