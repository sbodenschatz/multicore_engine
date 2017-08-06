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
unique_descriptor_pool::allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
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
		const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts, destruction_queue_manager* dqm) {
	std::vector<descriptor_set> rv;
	descriptor_set_resources req;
	for(const auto& layout : layouts) {
		req += *layout;
	}
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
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
								  const std::shared_ptr<descriptor_set_layout>& layout) {
	descriptor_set_resources alloc = *layout;
	std::lock_guard<std::mutex> lock(pool_mutex_);
	(*dev_)->freeDescriptorSets(native_pool_.get(), set);
	available_resources_ += alloc;
}

} /* namespace graphics */
} /* namespace mce */
