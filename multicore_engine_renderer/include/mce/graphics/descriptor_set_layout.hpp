/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set_layout.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_

#include <mce/graphics/destruction_queue_manager.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

class descriptor_set_layout {
public:
	struct binding_element {
		uint32_t binding;
		vk::DescriptorType descriptorType;
		uint32_t descriptorCount;
		vk::ShaderStageFlags stageFlags;
		std::vector<vk::Sampler> pImmutableSamplers;
	};

private:
	std::vector<binding_element> bindings_;
	queued_handle<vk::UniqueDescriptorSetLayout> native_layout_;

public:
	descriptor_set_layout();
	~descriptor_set_layout();

	const std::vector<binding_element>& bindings() const {
		return bindings_;
	}

	vk::DescriptorSetLayout native_layout() const {
		return native_layout_.get();
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_LAYOUT_HPP_ */
