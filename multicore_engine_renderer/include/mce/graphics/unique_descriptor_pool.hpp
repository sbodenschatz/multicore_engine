/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/unique_descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_

#include <boost/container/flat_map.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/util/array_utils.hpp>
#include <memory>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class descriptor_set;
class destruction_queue_manager;

class unique_descriptor_pool {
	mutable std::mutex pool_mutex_;
	device* dev_;
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

	void free(vk::DescriptorSet set);

public:
	unique_descriptor_pool(device& dev, uint32_t max_sets,
						   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes);
	~unique_descriptor_pool();

	unique_descriptor_pool(const unique_descriptor_pool&) = delete;
	unique_descriptor_pool& operator=(const unique_descriptor_pool&) = delete;
	unique_descriptor_pool(unique_descriptor_pool&&) = delete;
	unique_descriptor_pool& operator=(unique_descriptor_pool&&) = delete;

	uint32_t available_descriptors(vk::DescriptorType type) const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		auto it = available_pool_sizes_.find(type);
		if(it == available_pool_sizes_.end()) return 0;
		return it->second;
	}

	uint32_t available_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return available_sets_;
	}

	uint32_t max_descriptors(vk::DescriptorType type) const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		auto it = max_pool_sizes_.find(type);
		if(it == max_pool_sizes_.end()) return 0;
		return it->second;
	}

	uint32_t max_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return max_sets_;
	}

	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);

	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);

	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts,
							 destruction_queue_manager* dqm = nullptr) {
		std::array<vk::DescriptorSetLayout, size> nlayouts;
		std::array<vk::DescriptorSet, size> nsets;
		std::transform(layouts.begin(), layouts.end(), nlayouts.begin(),
					   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
		vk::DescriptorSetAllocateInfo ai(native_pool_.get(), size, nlayouts.data());
		{
			std::lock_guard<std::mutex> lock(pool_mutex_);
			auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
			if(res != vk::Result::eSuccess) {
				throw std::system_error(res, "vk::Device::allocateDescriptorSets");
			}
			for(const auto& layout : layouts) {
				for(const auto& elem : layout->bindings()) {
					available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
				}
			}
			available_sets_ -= uint32_t(layouts.size());
		}
		vk::DescriptorSetDeleter del(dev_->native_device(), native_pool_.get());
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[dqm, this, &del](vk::DescriptorSet ds, const std::shared_ptr<descriptor_set_layout>& l) {
					return descriptor_set(*dev_, dqm, vk::UniqueDescriptorSet(ds, del), l);
				});
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_ */
