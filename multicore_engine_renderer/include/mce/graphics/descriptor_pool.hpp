/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_

#include <boost/container/flat_map.hpp>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class device;

class descriptor_pool {
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

public:
	descriptor_pool(device& dev, uint32_t max_sets, vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes,
					bool set_freeable = false);
	~descriptor_pool();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_ */
