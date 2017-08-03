/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/descriptor_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/iterator/zip_iterator.hpp>
#include <iterator>
#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_pool.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>

namespace mce {
namespace graphics {

descriptor_pool::descriptor_pool(device& dev, uint32_t max_sets,
								 vk::ArrayProxy<const vk::DescriptorPoolSize> pool_sizes,
								 bool unique_allocation)
		: dev_{&dev}, unique_allocation_{unique_allocation}, max_sets_{max_sets}, available_sets_{max_sets} {
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
	native_pool_ = dev->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(
			unique_allocation ? vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet
							  : vk::DescriptorPoolCreateFlags(),
			max_sets_, uint32_t(pool_size_sums.size()), pool_size_sums.data()));
}

descriptor_pool::~descriptor_pool() {}

void descriptor_pool::reset() {
	if(unique_allocation_)
		throw mce::graphics_exception("Reset is not supported for unique_allocation enabled descriptor_pool "
									  "because it would interfere with the freeing through unique handles.");
	(*dev_)->resetDescriptorPool(native_pool_.get());
}

descriptor_set descriptor_pool::allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout,
														destruction_queue_manager* dqm) {
	auto nlayout = layout->native_layout();
	vk::DescriptorSetAllocateInfo ai(native_pool_.get(), 1, &nlayout);
	vk::DescriptorSet set;
	auto res = (*dev_)->allocateDescriptorSets(&ai, &set);
	vk::DescriptorSetDeleter del(dev_->native_device(), native_pool_.get());
	if(unique_allocation_) {
		return descriptor_set(
				dqm, vk::UniqueDescriptorSet(
							 vk::createResultValue(res, set, "vk::Device::allocateDescriptorSets"), del),
				layout);
	} else {
		return descriptor_set(vk::createResultValue(res, set, "vk::Device::allocateDescriptorSets"), layout);
	}
}

std::vector<descriptor_set>
descriptor_pool::allocate_descriptor_set(const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts,
										 destruction_queue_manager* dqm) {
	std::vector<descriptor_set> rv;
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });
	if(unique_allocation_) {
		auto tmp = (*dev_)->allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo(
				native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
		auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
		auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
		std::transform(beg, end, std::back_inserter(rv), [dqm](auto tup) {
			return descriptor_set(dqm, std::move(boost::get<0>(tup)), boost::get<1>(tup));
		});
	} else {
		auto tmp = (*dev_)->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(
				native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
		auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
		auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
		std::transform(beg, end, std::back_inserter(rv),
					   [dqm](auto tup) { return descriptor_set(boost::get<0>(tup), boost::get<1>(tup)); });
	}
	return rv;
}

} /* namespace graphics */
} /* namespace mce */
