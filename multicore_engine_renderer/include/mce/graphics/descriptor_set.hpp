/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_set.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_HPP_

#include <boost/optional.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set_layout;

class descriptor_set {
	boost::optional<vk::UniqueDescriptorSet> descriptor_set_unique;
	vk::DescriptorSet native_descriptor_set_;
	std::shared_ptr<descriptor_set_layout> layout_;

public:
	descriptor_set(vk::DescriptorSet native_descriptor_set, std::shared_ptr<descriptor_set_layout> layout);
	descriptor_set(vk::UniqueDescriptorSet native_descriptor_set,
				   std::shared_ptr<descriptor_set_layout> layout);
	~descriptor_set();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_HPP_ */
