/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/simple_descriptor_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/iterator/zip_iterator.hpp>
#include <iterator>
#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/simple_descriptor_pool.hpp>

namespace mce {
namespace graphics {

simple_descriptor_pool::simple_descriptor_pool(device& dev, uint32_t max_sets,
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
	native_pool_ = dev->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(
			{}, max_sets_, uint32_t(pool_size_sums.size()), pool_size_sums.data()));
}

simple_descriptor_pool::~simple_descriptor_pool() {}

simple_descriptor_pool::simple_descriptor_pool(simple_descriptor_pool&& other) noexcept
		: dev_{other.dev_}, native_pool_{std::move(other.native_pool_)}, max_sets_{other.max_sets_},
		  available_sets_{other.available_sets_}, max_pool_sizes_{std::move(other.max_pool_sizes_)},
		  available_pool_sizes_{std::move(other.available_pool_sizes_)} {
	other.dev_ = nullptr;
	other.max_sets_ = 0;
	other.available_sets_ = 0;
}
simple_descriptor_pool& simple_descriptor_pool::operator=(simple_descriptor_pool&& other) noexcept {
	dev_ = other.dev_;
	native_pool_ = std::move(other.native_pool_);
	max_sets_ = other.max_sets_;
	available_sets_ = other.available_sets_;
	max_pool_sizes_ = std::move(other.max_pool_sizes_);
	available_pool_sizes_ = std::move(other.available_pool_sizes_);
	other.dev_ = nullptr;
	other.max_sets_ = 0;
	other.available_sets_ = 0;
	return *this;
}

void simple_descriptor_pool::reset() {
	(*dev_)->resetDescriptorPool(native_pool_.get());
	available_sets_ = max_sets_;
	available_pool_sizes_ = max_pool_sizes_;
}

descriptor_set
simple_descriptor_pool::allocate_descriptor_set(const std::shared_ptr<descriptor_set_layout>& layout) {
	auto nlayout = layout->native_layout();
	vk::DescriptorSetAllocateInfo ai(native_pool_.get(), 1, &nlayout);
	vk::DescriptorSet set;
	auto res = (*dev_)->allocateDescriptorSets(&ai, &set);
	for(const auto& elem : layout->bindings()) {
		available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
	}
	available_sets_--;
	return descriptor_set(*dev_, vk::createResultValue(res, set, "vk::Device::allocateDescriptorSets"),
						  layout);
}

std::vector<descriptor_set> simple_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<descriptor_set_layout>>& layouts) {
	std::vector<descriptor_set> rv;
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<descriptor_set_layout>& l) { return l->native_layout(); });

	auto tmp = (*dev_)->allocateDescriptorSets(
			vk::DescriptorSetAllocateInfo(native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
	for(const auto& layout : layouts) {
		for(const auto& elem : layout->bindings()) {
			available_pool_sizes_.at(elem.descriptor_type) -= elem.descriptor_count;
		}
	}
	available_sets_ -= uint32_t(layouts.size());
	auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
	auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
	std::transform(beg, end, std::back_inserter(rv), [this](auto tup) {
		return descriptor_set(*dev_, boost::get<0>(tup), boost::get<1>(tup));
	});

	return rv;
}

} /* namespace graphics */
} /* namespace mce */
