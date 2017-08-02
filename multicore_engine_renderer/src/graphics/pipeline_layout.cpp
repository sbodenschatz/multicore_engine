/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/pipeline_layout.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <iterator>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline_layout.hpp>

namespace mce {
namespace graphics {

pipeline_layout::pipeline_layout(const device& dev, destruction_queue_manager* dqm,
								 std::vector<std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts,
								 std::vector<vk::PushConstantRange> push_constant_ranges)
		: push_constant_ranges_{std::move(push_constant_ranges)}, descriptor_set_layouts_{
																		  std::move(descriptor_set_layouts)} {
	boost::container::small_vector<vk::DescriptorSetLayout, 32> dsls;
	dsls.reserve(descriptor_set_layouts_.size());
	std::transform(descriptor_set_layouts_.begin(), descriptor_set_layouts_.end(), std::back_inserter(dsls),
				   [](const std::shared_ptr<descriptor_set_layout>& dsl) { return dsl->native_layout(); });
	native_layout_ = queued_handle<vk::UniquePipelineLayout>(
			dev->createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo(
					{}, uint32_t(dsls.size()), dsls.data(), uint32_t(push_constant_ranges_.size()),
					push_constant_ranges_.data())),
			dqm);
}

pipeline_layout::~pipeline_layout() {}

} /* namespace graphics */
} /* namespace mce */
