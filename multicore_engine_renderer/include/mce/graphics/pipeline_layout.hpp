/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/pipeline_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_
#define MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_

#include <mce/graphics/destruction_queue_manager.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

class descriptor_set_layout;

class pipeline_layout {
	std::vector<vk::PushConstantRange> push_constant_ranges_;
	std::vector<std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts_;
	queued_handle<vk::UniquePipelineLayout> native_layout_;

public:
	pipeline_layout();
	~pipeline_layout();

	const std::vector<std::shared_ptr<descriptor_set_layout>>& descriptor_set_layouts() const {
		return descriptor_set_layouts_;
	}

	vk::PipelineLayout native_layout() const {
		return native_layout_.get();
	}

	const std::vector<vk::PushConstantRange>& push_constant_ranges() const {
		return push_constant_ranges_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_ */
