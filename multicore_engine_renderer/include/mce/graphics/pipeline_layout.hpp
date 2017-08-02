/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/pipeline_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_
#define MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_

/**
 * \file
 * Defines the wrapper class for vulkan pipeline layouts.
 */

#include <mce/graphics/destruction_queue_manager.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set_layout;

/// Encapsulates a vulkan pipeline layout  and the associated data.
class pipeline_layout {
	std::vector<vk::PushConstantRange> push_constant_ranges_;
	std::vector<std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts_;
	queued_handle<vk::UniquePipelineLayout> native_layout_;

public:
	/// \brief Creates a pipeline_layout containing the given descriptor_set_layout objects and push constant
	/// ranges on the given device using the given destruction_queue_manager for resource destruction.
	pipeline_layout(const device& dev, destruction_queue_manager* dqm,
					std::vector<std::shared_ptr<descriptor_set_layout>> descriptor_set_layouts,
					std::vector<vk::PushConstantRange> push_constant_ranges = {});
	/// \brief Destroys the pipeline_layout wrapper and releases the underlying resources to the
	/// destruction_queue_manager given at construction.
	~pipeline_layout();

	/// Allows access to the descriptor_set_layout objects contained in the pipeline_layout.
	const std::vector<std::shared_ptr<descriptor_set_layout>>& descriptor_set_layouts() const {
		return descriptor_set_layouts_;
	}

	/// Allows access to the native vulkan pipeline layout.
	vk::PipelineLayout native_layout() const {
		return native_layout_.get();
	}

	/// Allows access to the push constant ranges contained in the pipeline_layout.
	const std::vector<vk::PushConstantRange>& push_constant_ranges() const {
		return push_constant_ranges_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_PIPELINE_LAYOUT_HPP_ */
