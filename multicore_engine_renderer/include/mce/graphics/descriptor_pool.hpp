/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_

/**
 * \file
 * Defines the wrapper class for vulkan descriptor pools.
 */

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

/// \brief Encapsulates a bounded pool for allocating descriptors for vulkan descriptor sets (represented by
/// descriptor_set objects).
/**
 * Supports either a mode where descriptor_set objects don't own the underlying resources but the pool owns
 * them and can be reset (unique_allocation) or a mode where the descriptor_set objects own them and the pool
 * can not be reset.
 */
class descriptor_pool {
	device* dev_;
	bool unique_allocation_;
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

public:
	/// \brief Creates a descriptor_pool on the given device for the given amount of sets and descriptors per
	/// type with the given mode.
	descriptor_pool(device& dev, uint32_t max_sets, vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes,
					bool unique_allocation = false);
	/// \brief Destroys the descriptor_pool, owning descriptor_set objects from it must be destroyed before
	/// it, non-owning sets become invalid.
	~descriptor_pool();

	/// Returns the number of available descriptors for the given type.
	uint32_t available_descriptors(vk::DescriptorType type) const {
		auto it = available_pool_sizes_.find(type);
		if(it == available_pool_sizes_.end()) return 0;
		return it->second;
	}

	/// Returns the number of available descriptor sets.
	uint32_t available_sets() const {
		return available_sets_;
	}

	/// Returns the capacity for descriptors of the given type.
	uint32_t max_descriptors(vk::DescriptorType type) const {
		auto it = max_pool_sizes_.find(type);
		if(it == max_pool_sizes_.end()) return 0;
		return it->second;
	}

	/// Returns the capacity for descriptor sets.
	uint32_t max_sets() const {
		return max_sets_;
	}

	/// Allows access to the native vulkan descriptor pool handle.
	vk::DescriptorPool native_pool() const {
		return native_pool_.get();
	}

	/// Allows access to the available pool sizes map.
	const boost::container::flat_map<vk::DescriptorType, uint32_t>& available_pool_sizes() const {
		return available_pool_sizes_;
	}

	/// Allows access to the max pool sizes map.
	const boost::container::flat_map<vk::DescriptorType, uint32_t>& max_pool_sizes() const {
		return max_pool_sizes_;
	}

	/// \brief Allocates a descriptor set of the given layout, optionally using the given
	/// destruction_queue_manager for an owning descriptor_set.
	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);

	/// \brief Allocates descriptor sets of the given layouts, optionally using the given
	/// destruction_queue_manager for a owning descriptor_set objects.
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);

	/// \brief Allocates descriptor sets of the given layouts, optionally using the given
	/// destruction_queue_manager for a owning descriptor_set objects.
	/**
	 * Avoids heap allocations by statically determining the number of sets.
	 */
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts,
							 destruction_queue_manager* dqm = nullptr) {
		std::array<vk::DescriptorSetLayout, size> nlayouts;
		std::array<vk::DescriptorSet, size> nsets;
		std::transform(layouts.begin(), layouts.end(), nlayouts.begin(),
					   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
		vk::DescriptorSetAllocateInfo ai(native_pool_.get(), size, nlayouts.data());
		auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateDescriptorSets");
		}
		vk::DescriptorSetDeleter del(dev_->native_device(), native_pool_.get());
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[dqm, this, &del](vk::DescriptorSet ds, const std::shared_ptr<descriptor_set_layout>& l) {
					if(unique_allocation_) {
						return descriptor_set(*dev_, dqm, vk::UniqueDescriptorSet(ds, del), l);
					} else {
						return descriptor_set(*dev_, ds, l);
					}
				});
	}

	/// Resets the descriptor_pool, invalidating all descriptor sets allocated from it.
	/**
	 * This is only supported if the pool was constructed with unique_allocation = false.
	 */
	void reset();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_DESCRIPTOR_POOL_HPP_ */
