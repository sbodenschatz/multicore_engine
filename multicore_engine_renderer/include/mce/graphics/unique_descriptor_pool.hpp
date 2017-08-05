/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/unique_descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_

/**
 * \file
 * Defines the simple_descriptor_pool class.
 */

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

/// \brief Encapsulates a bounded pool for allocating descriptors for vulkan descriptor sets (represented by
/// descriptor_set objects).
/**
 * The unique variation of descriptor pools allocates the descriptor sets as RAII-owners and manages resources
 * in a thread-safe manner. To not interfere with RAII-ownership this variation doesn't support explicitly
 * resetting the pool. For thread-safety and reference integrity of the descriptor set destruction this
 * variation also doesn't support move operations.
 *
 * This variation is intended for long-lived and / or not thread-specific descriptors.
 */
class unique_descriptor_pool {
	mutable std::mutex pool_mutex_;
	device* dev_;
	vk::UniqueDescriptorPool native_pool_;
	uint32_t max_sets_;
	uint32_t available_sets_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> max_pool_sizes_;
	boost::container::flat_map<vk::DescriptorType, uint32_t> available_pool_sizes_;

	friend class descriptor_set_deleter;

	void free(vk::DescriptorSet set, const std::shared_ptr<descriptor_set_layout>& layout);

public:
	/// \brief Creates a unique_descriptor_pool for the given device with the given number sets and the given
	/// numbers descriptors for each type.
	unique_descriptor_pool(device& dev, uint32_t max_sets,
						   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes);
	/// \brief Destroys the unique_descriptor_pool and releases the underlying resources.
	/**
	 * Requires that all descriptor_set objects allocated from this pool must be destroyed before it.
	 */
	~unique_descriptor_pool();

	/// Explicitly forbids copying.
	unique_descriptor_pool(const unique_descriptor_pool&) = delete;
	/// Explicitly forbids copying.
	unique_descriptor_pool& operator=(const unique_descriptor_pool&) = delete;
	/// Explicitly forbids moving.
	unique_descriptor_pool(unique_descriptor_pool&&) = delete;
	/// Explicitly forbids moving.
	unique_descriptor_pool& operator=(unique_descriptor_pool&&) = delete;

	/// Returns the number of available descriptors for the given type in the pool.
	uint32_t available_descriptors(vk::DescriptorType type) const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		auto it = available_pool_sizes_.find(type);
		if(it == available_pool_sizes_.end()) return 0;
		return it->second;
	}

	/// Returns the number of available descriptor sets in the pool.
	uint32_t available_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return available_sets_;
	}

	/// Returns the total number of descriptors for the given type in the pool.
	uint32_t max_descriptors(vk::DescriptorType type) const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		auto it = max_pool_sizes_.find(type);
		if(it == max_pool_sizes_.end()) return 0;
		return it->second;
	}

	/// Returns the total number of descriptor sets in the pool.
	uint32_t max_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return max_sets_;
	}

	/// \brief Returns the remaining number of resources (sets or descriptors) for the resource that is
	/// closest to being depleted.
	uint32_t min_available_resource_amount() const;

	/// Allocates a descriptor set of the given layout.
	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);

	/// Allocates descriptor sets for each of the given layouts.
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);

	/// Allocates descriptor sets for each of the given layouts.
	/**
	 * Avoids heap allocations in the wrapper by statically determining the number of sets.
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
		{
			std::lock_guard<std::mutex> lock(pool_mutex_);
			auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
			if(res != vk::Result::eSuccess) {
				throw std::system_error(res, "vk::Device::allocateDescriptorSets");
			}
			try {
				for(const auto& layout : layouts) {
					for(const auto& elem : layout->bindings()) {
						available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
					}
				}
			} catch(...) {
				(*dev_)->freeDescriptorSets(native_pool_.get(), nsets);
				throw;
			}
			available_sets_ -= uint32_t(layouts.size());
		}
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[dqm, this](vk::DescriptorSet ds, const std::shared_ptr<descriptor_set_layout>& l) {
					return descriptor_set(*dev_, ds, this, dqm, l);
				});
	}
};

class growing_unique_descriptor_pool {
	mutable std::mutex blocks_mutex_;
	device* dev_;
	uint32_t block_sets_;
	std::vector<vk::DescriptorPoolSize> block_pool_sizes_;
	std::vector<simple_descriptor_pool> blocks_;

public:
	growing_unique_descriptor_pool(device& dev, uint32_t descriptor_sets_per_block,
								   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes_per_block);
	growing_unique_descriptor_pool(device& dev, uint32_t descriptor_sets_per_block,
								   std::vector<vk::DescriptorPoolSize> pool_sizes_per_block);

	uint32_t available_descriptors(vk::DescriptorType type) const;
	uint32_t available_sets() const;

	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts,
							 destruction_queue_manager* dqm = nullptr);
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_ */
