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

unique_descriptor_pool::unique_descriptor_pool(device& dev, uint32_t max_sets,
											   vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes)
		: dev_{&dev}, max_sets_{max_sets}, available_sets_{max_sets} {
	max_pool_sizes_.reserve(pool_sizes.size());
	for(const vk::DescriptorPoolSize& dps : pool_sizes) {
		max_pool_sizes_[dps.type] += dps.descriptorCount;
	}
	available_pool_sizes_ = max_pool_sizes_;
	boost::container::small_vector<vk::DescriptorPoolSize, 16> pool_size_sums;
	std::transform(max_pool_sizes_.begin(), max_pool_sizes_.end(), std::back_inserter(pool_size_sums),
				   [](const std::pair<vk::DescriptorType, uint32_t>& ps) {
					   return vk::DescriptorPoolSize(ps.first, ps.second);
				   });
	native_pool_ = dev->createDescriptorPoolUnique(
			vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, max_sets_,
										 uint32_t(pool_size_sums.size()), pool_size_sums.data()));
}

unique_descriptor_pool::~unique_descriptor_pool() {}

descriptor_set
unique_descriptor_pool::allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
												destruction_queue_manager* dqm) {
	auto nlayout = layout->native_layout();
	vk::DescriptorSetAllocateInfo ai(native_pool_.get(), 1, &nlayout);
	vk::DescriptorSet set;
	{
		std::lock_guard<std::mutex> lock(pool_mutex_);
		auto res = (*dev_)->allocateDescriptorSets(&ai, &set);
		if(res != vk::Result::eSuccess) {
			throw std::system_error(res, "vk::Device::allocateDescriptorSets");
		}
		for(const auto& elem : layout->bindings()) {
			available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
		}
		available_sets_--;
	}
	vk::DescriptorSetDeleter del(dev_->native_device(), native_pool_.get());
	return descriptor_set(*dev_, dqm, vk::UniqueDescriptorSet(set, del), layout);
}

std::vector<descriptor_set> unique_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts, destruction_queue_manager* dqm) {
	std::vector<descriptor_set> rv;
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
	std::unique_lock<std::mutex> lock(pool_mutex_);
	auto tmp = (*dev_)->allocateDescriptorSetsUnique(
			vk::DescriptorSetAllocateInfo(native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
	for(const auto& layout : layouts) {
		for(const auto& elem : layout->bindings()) {
			available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
		}
	}
	available_sets_ -= uint32_t(layouts.size());
	lock.unlock();
	auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
	auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
	std::transform(beg, end, std::back_inserter(rv), [this, dqm](auto tup) {
		return descriptor_set(*dev_, dqm, std::move(boost::get<0>(tup)), boost::get<1>(tup));
	});
	return rv;
}

} /* namespace graphics */
} /* namespace mce */
