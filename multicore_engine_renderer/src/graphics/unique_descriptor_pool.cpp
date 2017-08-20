/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/unique_descriptor_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/iterator/zip_iterator.hpp>
#include <iterator>
#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/unique_descriptor_pool.hpp>
#include <numeric>

namespace mce {
namespace graphics {

unique_descriptor_pool::unique_descriptor_pool(device& dev, descriptor_set_resources capacity)
		: dev_{&dev}, max_resources_{std::move(capacity)}, available_resources_{max_resources_} {
	boost::container::small_vector<vk::DescriptorPoolSize, 16> pool_size_sums;
	std::transform(max_resources_.descriptors().begin(), max_resources_.descriptors().end(),
				   std::back_inserter(pool_size_sums), [](const std::pair<vk::DescriptorType, uint32_t>& ps) {
					   return vk::DescriptorPoolSize(ps.first, ps.second);
				   });
	native_pool_ = dev->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(
			vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, max_resources_.descriptor_sets(),
			uint32_t(pool_size_sums.size()), pool_size_sums.data()));
}

unique_descriptor_pool::~unique_descriptor_pool() {}

descriptor_set
unique_descriptor_pool::allocate_descriptor_set(const std::shared_ptr<const descriptor_set_layout>& layout,
												destruction_queue_manager* dqm) {
	descriptor_set_resources req = *layout;
	auto nlayout = layout->native_layout();
	vk::DescriptorSetAllocateInfo ai(native_pool_.get(), 1, &nlayout);
	vk::DescriptorSet set;
	{
		std::lock_guard<std::mutex> lock(pool_mutex_);
		if(!available_resources_.sufficient_for(req)) {
			throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
		}
		auto res = (*dev_)->allocateDescriptorSets(&ai, &set);
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateDescriptorSets");
		}
		available_resources_ -= req;
	}
	return descriptor_set(*dev_, set, this, dqm, layout);
}

std::vector<descriptor_set> unique_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts,
		destruction_queue_manager* dqm) {
	std::vector<descriptor_set> rv;
	descriptor_set_resources req;
	for(const auto& layout : layouts) {
		req += *layout;
	}
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<const descriptor_set_layout>& l) { return l->native_layout(); });
	std::unique_lock<std::mutex> lock(pool_mutex_);
	if(!available_resources_.sufficient_for(req)) {
		throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
	}
	auto tmp = (*dev_)->allocateDescriptorSets(
			vk::DescriptorSetAllocateInfo(native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
	available_resources_ -= req;
	lock.unlock();
	auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
	auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
	std::transform(beg, end, std::back_inserter(rv), [this, dqm](auto tup) {
		return descriptor_set(*dev_, boost::get<0>(tup), this, dqm, boost::get<1>(tup));
	});
	return rv;
}

void unique_descriptor_pool::free(vk::DescriptorSet set,
								  const std::shared_ptr<const descriptor_set_layout>& layout) {
	descriptor_set_resources alloc = *layout;
	std::lock_guard<std::mutex> lock(pool_mutex_);
	(*dev_)->freeDescriptorSets(native_pool_.get(), set);
	available_resources_ += alloc;
}

growing_unique_descriptor_pool::growing_unique_descriptor_pool(device& dev,
															   descriptor_set_resources block_resources)
		: dev_{&dev}, block_resources_{std::move(block_resources)} {
	blocks_.emplace_back(std::make_unique<unique_descriptor_pool>(*dev_, block_resources_));
}

uint32_t growing_unique_descriptor_pool::available_descriptors(vk::DescriptorType type) const {
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	return std::accumulate(blocks_.begin(), blocks_.end(), 0u,
						   [type](uint32_t s, const std::unique_ptr<unique_descriptor_pool>& p) {
							   return s + p->available_descriptors(type);
						   });
}

uint32_t growing_unique_descriptor_pool::available_sets() const {
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	return std::accumulate(blocks_.begin(), blocks_.end(), 0u,
						   [](uint32_t s, const std::unique_ptr<unique_descriptor_pool>& p) {
							   return s + p->available_sets();
						   });
}

descriptor_set growing_unique_descriptor_pool::allocate_descriptor_set(
		const std::shared_ptr<const descriptor_set_layout>& layout, destruction_queue_manager* dqm) {
	descriptor_set_resources req = *layout;
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	auto it = std::find_if(blocks_.begin(), blocks_.end(),
						   [&req](const std::unique_ptr<unique_descriptor_pool>& blk) {
							   return blk->available_resources().sufficient_for(req);
						   });
	if(it != blocks_.end()) {
		return (*it)->allocate_descriptor_set(layout, dqm);
	} else {
		blocks_.emplace_back(std::make_unique<unique_descriptor_pool>(*dev_, block_resources_));
		return blocks_.back()->allocate_descriptor_set(layout, dqm);
	}
}

std::vector<descriptor_set> growing_unique_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts,
		destruction_queue_manager* dqm) {
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

uint32_t growing_unique_descriptor_pool::descriptors_capacity(vk::DescriptorType type) const {
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	return std::accumulate(blocks_.begin(), blocks_.end(), 0u,
						   [type](uint32_t s, const std::unique_ptr<unique_descriptor_pool>& p) {
							   return s + p->max_descriptors(type);
						   });
}
uint32_t growing_unique_descriptor_pool::sets_capacity() const {
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	return std::accumulate(
			blocks_.begin(), blocks_.end(), 0u,
			[](uint32_t s, const std::unique_ptr<unique_descriptor_pool>& p) { return s + p->max_sets(); });
}

descriptor_set_resources growing_unique_descriptor_pool::available_resources() const {
	descriptor_set_resources rv;
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	for(const auto& blk : blocks_) {
		rv += blk->available_resources();
	}
	return rv;
}
descriptor_set_resources growing_unique_descriptor_pool::resource_capacity() const {
	descriptor_set_resources rv;
	std::lock_guard<std::mutex> lock(blocks_mutex_);
	for(const auto& blk : blocks_) {
		rv += blk->max_resources();
	}
	return rv;
}

} /* namespace graphics */
} /* namespace mce */
