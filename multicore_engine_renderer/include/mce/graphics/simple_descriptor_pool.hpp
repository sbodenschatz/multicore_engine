/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_

#include <boost/container/flat_map.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/util/array_utils.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set;
class destruction_queue_manager;

class simple_descriptor_pool {
	device* dev_;
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

public:
	simple_descriptor_pool(device& dev, uint32_t max_sets,
						   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes);
	~simple_descriptor_pool();

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

	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout);

	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts);

	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts) {
		std::array<vk::DescriptorSetLayout, size> nlayouts;
		std::array<vk::DescriptorSet, size> nsets;
		std::transform(layouts.begin(), layouts.end(), nlayouts.begin(),
					   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
		vk::DescriptorSetAllocateInfo ai(native_pool_.get(), size, nlayouts.data());
		auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
		for(const auto& layout : layouts) {
			for(const auto& elem : layout->bindings()) {
				available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
			}
		}
		available_sets_ -= uint32_t(layouts.size());
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateDescriptorSets");
		}
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[this](vk::DescriptorSet ds, const std::shared_ptr<descriptor_set_layout>& l) {
					return descriptor_set(*dev_, ds, l);

				});
	}

	void reset();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_ */
