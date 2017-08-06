/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_descriptor_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_
#define MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_

/**
 * \file
 * Defines the simple_descriptor_pool class.
 */

#include <boost/container/flat_map.hpp>
#include <mce/graphics/descriptor_pool_resources.hpp>
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
 * The simple variation of the descriptor pool works with non-owning descriptor sets. This means that the
 * objects are allocated from the pool but are not RAII-owners of the underlying resources but are invalidated
 * when all resources are taken back to the pool by resetting the pool. Additionally the
 * simple_descriptor_pool is not thread-safe.
 *
 * This variation is intended for per-thread and per-frame usage and provides lower overhead for this scenario
 * than the other variation unique_descriptor_pool.
 */
class simple_descriptor_pool {
	device* dev_;
	vk::UniqueDescriptorPool native_pool_;
	descriptor_set_resources max_resources_;
	descriptor_set_resources available_resources_;

public:
	/// \brief Creates a simple_descriptor_pool for the given device with the given number sets and the given
	/// numbers descriptors for each type.
	simple_descriptor_pool(device& dev, uint32_t max_sets,
						   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes)
			: simple_descriptor_pool(dev, descriptor_set_resources{pool_sizes, max_sets}) {}

	simple_descriptor_pool(device& dev, descriptor_set_resources capacity);

	/// \brief Destroys the simple_descriptor_pool, releases the underlying resources and makes all descriptor
	/// sets allocated from it invalid.
	~simple_descriptor_pool();

	/// Explicitly forbids copying.
	simple_descriptor_pool(const simple_descriptor_pool&) = delete;
	/// Explicitly forbids copying.
	simple_descriptor_pool& operator=(const simple_descriptor_pool&) = delete;
	/// Allows move-construction.
	simple_descriptor_pool(simple_descriptor_pool&&) noexcept;
	/// Allows mode-assignment.
	simple_descriptor_pool& operator=(simple_descriptor_pool&&) noexcept;

	/// Returns the number of available descriptors for the given type in the pool.
	uint32_t available_descriptors(vk::DescriptorType type) const {
		return available_resources_.descriptors(type);
	}

	/// Returns the number of available descriptor sets in the pool.
	uint32_t available_sets() const {
		return available_resources_.descriptor_sets();
	}

	/// Returns the total number of descriptors for the given type in the pool.
	uint32_t max_descriptors(vk::DescriptorType type) const {
		return max_resources_.descriptors(type);
	}

	/// Returns the total number of descriptor sets in the pool.
	uint32_t max_sets() const {
		return max_resources_.descriptor_sets();
	}

	/// Returns a description of all available resources in the pool.
	const descriptor_set_resources& available_resources() const {
		return available_resources_;
	}

	/// Returns a description of the resource capacity of the pool.
	const descriptor_set_resources& max_resources() const {
		return max_resources_;
	}

	/// \brief Returns the remaining number of resources (sets or descriptors) for the resource that is
	/// closest to being depleted.
	uint32_t min_available_resource_amount() const {
		return available_resources_.min();
	}

	/// Allocates a descriptor set of the given layout.
	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout);

	/// Allocates descriptor sets for each of the given layouts.
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts);

	/// Allocates descriptor sets for each of the given layouts.
	/**
	 * Avoids heap allocations in the wrapper by statically determining the number of sets.
	 */
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts) {
		descriptor_set_resources req;
		for(const auto& layout : layouts) {
			req += *layout;
		}
		if(!available_resources_.sufficient_for(req)) {
			throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
		}
		std::array<vk::DescriptorSetLayout, size> nlayouts;
		std::array<vk::DescriptorSet, size> nsets;
		std::transform(layouts.begin(), layouts.end(), nlayouts.begin(),
					   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
		vk::DescriptorSetAllocateInfo ai(native_pool_.get(), size, nlayouts.data());
		auto res = (*dev_)->allocateDescriptorSets(&ai, nsets.data());
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateDescriptorSets");
		}
		available_resources_ -= req;
		return mce::util::array_transform<descriptor_set>(
				nsets, layouts,
				[this](vk::DescriptorSet ds, const std::shared_ptr<descriptor_set_layout>& l) {
					return descriptor_set(*dev_, ds, l);

				});
	}

	/// \brief Resets the descriptor_pool, invalidating all descriptor sets allocated from it and returning
	/// the resources to the pool.
	void reset();
};

class growing_simple_descriptor_pool {
	device* dev_;
	descriptor_set_resources block_resources_;
	std::vector<simple_descriptor_pool> blocks_;

public:
	growing_simple_descriptor_pool(device& dev, uint32_t descriptor_sets_per_block,
								   std::vector<vk::DescriptorPoolSize> pool_sizes_per_block)
			: growing_simple_descriptor_pool(
					  dev, descriptor_set_resources{pool_sizes_per_block, descriptor_sets_per_block}) {}

	growing_simple_descriptor_pool(device& dev, descriptor_set_resources block_resources);

	uint32_t available_descriptors(vk::DescriptorType type) const;
	uint32_t available_sets() const;

	descriptor_set allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout);
	std::vector<descriptor_set>
	allocate_descriptor_sets(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts);
	template <size_t size>
	std::array<descriptor_set, size>
	allocate_descriptor_sets(const std::array<std::shared_ptr<descriptor_set_layout>, size>& layouts) {
		descriptor_set_resources req;
		for(const auto& layout : layouts) {
			req += *layout;
		}
		auto it = std::find_if(blocks_.begin(), blocks_.end(), [&req](const simple_descriptor_pool& blk) {
			return blk.available_resources().sufficient_for(req);
		});
		if(it != blocks_.end()) {
			return it->allocate_descriptor_sets(layouts);
		} else {
			blocks_.emplace_back(*dev_, block_resources_);
			return blocks_.back().allocate_descriptor_sets(layouts);
		}
	}
	void reset();
	void reset_and_shrink();
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_DESCRIPTOR_POOL_HPP_ */
