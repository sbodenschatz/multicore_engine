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
	bool unique_allocation_;
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

public:
	descriptor_pool(device& dev, uint32_t max_sets, vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes,
					bool unique_allocation = false);
	~descriptor_pool();

	uint32_t available_descriptors(vk::DescriptorType type) const {
		auto it = available_pool_sizes_.find(type);
		if(it == available_pool_sizes_.end()) return 0;
		return it->second;
	}

	uint32_t available_sets() const {
		return available_sets_;
	}

	uint32_t max_descriptors(vk::DescriptorType type) const {
		auto it = max_pool_sizes_.find(type);
		if(it == max_pool_sizes_.end()) return 0;
		return it->second;
	}

	uint32_t max_sets() const {
		return max_sets_;
	}

	vk::DescriptorPool native_pool() const {
		return native_pool_.get();
	}

	const boost::container::flat_map<vk::DescriptorType, uint32_t>& available_pool_sizes() const {
		return available_pool_sizes_;
	}

	const boost::container::flat_map<vk::DescriptorType, uint32_t>& max_pool_sizes() const {
		return max_pool_sizes_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_ */
