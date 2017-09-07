/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/unique_descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_

/**
 * \file
 * Defines the simple_descriptor_pool and growing_unique_descriptor_pool classes.
 */

#include <boost/container/flat_map.hpp>
#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/descriptor_set_resources.hpp>
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
	descriptor_set_resources max_resources_;
	descriptor_set_resources available_resources_;

	friend class descriptor_set_deleter;

	void free(vk::DescriptorSet set, const std::shared_ptr<const descriptor_set_layout>& layout);

public:
	/// \brief Creates a unique_descriptor_pool for the given device with the given number sets and the given
	/// numbers descriptors for each type.
	unique_descriptor_pool(device& dev, uint32_t max_sets,
						   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes)
			: unique_descriptor_pool(dev, descriptor_set_resources{pool_sizes, max_sets}) {}

	/// Creates a unique_descriptor_pool for the given device with the given resource amounts.
	unique_descriptor_pool(device& dev, descriptor_set_resources capacity);

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
		return available_resources_.descriptors(type);
	}

	/// Returns the number of available descriptor sets in the pool.
	uint32_t available_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return available_resources_.descriptor_sets();
	}

	/// Returns the total number of descriptors for the given type in the pool.
	uint32_t max_descriptors(vk::DescriptorType type) const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return max_resources_.descriptors(type);
	}

	/// Returns the total number of descriptor sets in the pool.
	uint32_t max_sets() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return max_resources_.descriptor_sets();
	}

	/// Returns a description of all available resources in the pool.
	descriptor_set_resources available_resources() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return available_resources_;
	}

	/// Returns a description of the resource capacity of the pool.
	descriptor_set_resources max_resources() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return max_resources_;
	}

	/// \brief Returns the remaining number of resources (sets or descriptors) for the resource that is
	/// closest to being depleted.
	uint32_t min_available_resource_amount() const {
		std::lock_guard<std::mutex> lock(pool_mutex_);
		return available_resources_.min_resource();
	}

	/// \brief Allocates a descriptor set of the given layout optionally using the given
	/// destruction_queue_manager for queued destruction.
	descriptor_set allocate_descriptor_set(const std::shared_ptr<const descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);

	///\brief Allocates descriptor sets for each of the given layouts optionally using the given
	/// destruction_queue_manager for queued destruction.
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);

	/// \brief Allocates descriptor sets for each of the given layouts optionally using the given
	/// destruction_queue_manager for queued destruction.
	/**
	 * Avoids heap allocations in the wrapper by statically determining the number of sets.
	 */
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<const descriptor_set_layout>, size>& layouts,
							 destruction_queue_manager* dqm = nullptr) {
		descriptor_set_resources req;
		for(const auto& layout : layouts) {
			req += *layout;
		}
		std::array<vk::DescriptorSetLayout, size> nlayouts;
		std::array<vk::DescriptorSet, size> nsets;
		std::transform(
				layouts.begin(), layouts.end(), nlayouts.begin(),
				[](const std::shared_ptr<const descriptor_set_layout>& l) { return l->native_layout(); });
		vk::DescriptorSetAllocateInfo ai(native_pool_.get(), size, nlayouts.data());
		{
			std::lock_guard<std::mutex> lock(pool_mutex_);
			if(!available_resources_.sufficient_for(req)) {
				throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
			}
			auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
			if(res != vk::Result::eSuccess) {
				throw std::system_error(res, "vk::Device::allocateDescriptorSets");
			}
			available_resources_ -= req;
		}
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[dqm, this](vk::DescriptorSet ds, const std::shared_ptr<const descriptor_set_layout>& l) {
					return descriptor_set(*dev_, ds, this, dqm, l);
				});
	}
};

/// \brief Extends the functionality of unique_description_pool by using multiple of them to implement
/// block-wise growth of the pool.
/**
 * Otherwise follows the same principles as in relying on RAII-ownership and providing thread-safety.
 */
class growing_unique_descriptor_pool {
	mutable std::mutex blocks_mutex_;
	device* dev_;
	descriptor_set_resources block_resources_;
	std::vector<std::unique_ptr<unique_descriptor_pool>> blocks_;

public:
	/// Creates a pool for the given device with the given resource amounts per block.
	growing_unique_descriptor_pool(device& dev, uint32_t descriptor_sets_per_block,
								   // cppcheck-suppress passedByValue
								   std::vector<vk::DescriptorPoolSize> pool_sizes_per_block)
			: growing_unique_descriptor_pool(
					  dev, descriptor_set_resources{pool_sizes_per_block, descriptor_sets_per_block}) {}

	/// Creates a pool for the given device with the given resource amounts per block.
	growing_unique_descriptor_pool(device& dev, descriptor_set_resources block_resources);

	/// \brief Returns the approximate total amount of descriptors available in the pool before another block
	/// needs to be allocated.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	uint32_t available_descriptors(vk::DescriptorType type) const;

	/// \brief Returns the approximate total amount of descriptor sets available in the pool before another
	/// block needs to be allocated.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	uint32_t available_sets() const;

	/// Returns the approximate current total capacity for descriptors of the given type.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	uint32_t descriptors_capacity(vk::DescriptorType type) const;
	/// Returns the approximate current total capacity for descriptor sets.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	uint32_t sets_capacity() const;

	/// \brief Returns the approximate total amount of resources available in the pool before another block
	/// needs to be allocated.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	descriptor_set_resources available_resources() const;
	/// Returns the approximate current total resource capacity of the pool.
	/**
	 * The amount is only approximate because concurrent descriptor_set destructions only lock the block from
	 * which they were allocated and not the growing pool.
	 * Because this function locks the growing pool for the full duration and each block separately while
	 * iterating over them the actual amount allocations can not take place concurrently with this function
	 * but free operations can. As a result the reported amount can be lower but not higher than the actual
	 * amount.
	 *
	 * However since anything can happen immediately after dropping the growing pool lock, checking this e.g.
	 * to make allocation decisions is problematic even if this were not just approximate.
	 *
	 * Therefore this function is mainly useful for statistic purposes.
	 */
	descriptor_set_resources resource_capacity() const;

	/// \brief Allocates a descriptor set of the given layout optionally using the given
	/// destruction_queue_manager for queued destruction.
	/**
	 * The required resources must not exceed the resources per block.
	 */
	descriptor_set allocate_descriptor_set(const std::shared_ptr<const descriptor_set_layout>& layout,
										   destruction_queue_manager* dqm = nullptr);

	/// \brief Allocates descriptor sets for each of the given layouts optionally using the given
	/// destruction_queue_manager for queued destruction.
	/**
	 * The required resources must not exceed the resources per block. Splitting the requests over multiple
	 * blocks is not supported to reduce overhead.
	 */
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts,
							 destruction_queue_manager* dqm = nullptr);

	/// \brief Allocates descriptor sets for each of the given layouts optionally using the given
	/// destruction_queue_manager for queued destruction.
	/**
	 * Reduces heap allocations in the wrapper by statically determining the number of sets.
	 *
	 * The required resources must not exceed the resources per block. Splitting the requests over multiple
	 * blocks is not supported to reduce overhead.
	 */
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<const descriptor_set_layout>, size>& layouts,
							 destruction_queue_manager* dqm = nullptr) {
		descriptor_set_resources req;
		for(const auto& layout : layouts) {
			req += *layout;
		}
		std::lock_guard<std::mutex> lock(blocks_mutex_);
		auto it = std::find_if(blocks_.begin(), blocks_.end(),
							   [&req](const std::unique_ptr<unique_descriptor_pool>& blk) {
								   return blk->available_resources().sufficient_for(req);
							   });
		if(it != blocks_.end()) {
			return (*it)->allocate_descriptor_sets(layouts, dqm);
		} else {
			blocks_.emplace_back(std::make_unique<unique_descriptor_pool>(*dev_, block_resources_));
			return blocks_.back()->allocate_descriptor_sets(layouts, dqm);
		}
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_UNIQUE_DESCRIPTOR_POOL_HPP_ */
