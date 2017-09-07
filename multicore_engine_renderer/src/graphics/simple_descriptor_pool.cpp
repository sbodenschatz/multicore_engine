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
#include <numeric>

namespace mce {
namespace graphics {

simple_descriptor_pool::simple_descriptor_pool(device& dev, descriptor_set_resources capacity)
		: dev_{&dev}, max_resources_{std::move(capacity)}, available_resources_{max_resources_} {
	boost::container::small_vector<vk::DescriptorPoolSize, 16> pool_size_sums;
	std::transform(max_resources_.descriptors().begin(), max_resources_.descriptors().end(),
				   std::back_inserter(pool_size_sums), [](const std::pair<vk::DescriptorType, uint32_t>& ps) {
					   return vk::DescriptorPoolSize(ps.first, ps.second);
				   });
	native_pool_ = dev->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(
			{}, max_resources_.descriptor_sets(), uint32_t(pool_size_sums.size()), pool_size_sums.data()));
}

simple_descriptor_pool::~simple_descriptor_pool() {}

simple_descriptor_pool::simple_descriptor_pool(simple_descriptor_pool&& other) noexcept
		: dev_{other.dev_}, native_pool_{std::move(other.native_pool_)},
		  max_resources_{std::move(other.max_resources_)}, available_resources_{
																   std::move(other.available_resources_)} {
	other.dev_ = nullptr;
}
simple_descriptor_pool& simple_descriptor_pool::operator=(simple_descriptor_pool&& other) noexcept {
	dev_ = other.dev_;
	native_pool_ = std::move(other.native_pool_);
	max_resources_ = std::move(other.max_resources_);
	available_resources_ = std::move(other.available_resources_);
	other.dev_ = nullptr;
	return *this;
}

void simple_descriptor_pool::reset() {
	(*dev_)->resetDescriptorPool(native_pool_.get());
	available_resources_ = max_resources_;
}

descriptor_set
simple_descriptor_pool::allocate_descriptor_set(const std::shared_ptr<const descriptor_set_layout>& layout,
												bool store_layout) {
	descriptor_set_resources req(*layout);
	auto nlayout = layout->native_layout();
	vk::DescriptorSetAllocateInfo ai(native_pool_.get(), 1, &nlayout);
	vk::DescriptorSet set;
	if(!available_resources_.sufficient_for(req)) {
		throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
	}
	auto res = (*dev_)->allocateDescriptorSets(&ai, &set);
	available_resources_ -= req;
	return descriptor_set(*dev_, vk::createResultValue(res, set, "vk::Device::allocateDescriptorSets"),
						  store_layout ? layout : std::shared_ptr<descriptor_set_layout>());
}

std::vector<descriptor_set> simple_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts, bool store_layout) {
	std::vector<descriptor_set> rv;
	descriptor_set_resources req;
	for(const auto& layout : layouts) {
		req += *layout;
	}
	if(!available_resources_.sufficient_for(req)) {
		throw mce::graphics_exception("Insufficient resources in pool for requested allocation.");
	}
	std::vector<vk::DescriptorSetLayout> nlayouts;
	rv.reserve(layouts.size());
	nlayouts.reserve(layouts.size());
	std::transform(layouts.begin(), layouts.end(), std::back_inserter(nlayouts),
				   [](const std::shared_ptr<const descriptor_set_layout>& l) { return l->native_layout(); });
	auto tmp = (*dev_)->allocateDescriptorSets(
			vk::DescriptorSetAllocateInfo(native_pool_.get(), uint32_t(nlayouts.size()), nlayouts.data()));
	available_resources_ -= req;
	auto beg = boost::make_zip_iterator(boost::make_tuple(tmp.begin(), layouts.begin()));
	auto end = boost::make_zip_iterator(boost::make_tuple(tmp.end(), layouts.end()));
	std::transform(beg, end, std::back_inserter(rv), [this, store_layout](auto tup) {
		return descriptor_set(*dev_, boost::get<0>(tup),
							  store_layout ? boost::get<1>(tup)
										   : std::shared_ptr<const descriptor_set_layout>());
	});

	return rv;
}

growing_simple_descriptor_pool::growing_simple_descriptor_pool(device& dev,
															   descriptor_set_resources block_resources)
		: dev_{&dev}, block_resources_{std::move(block_resources)} {
	blocks_.emplace_back(*dev_, block_resources_);
}

uint32_t growing_simple_descriptor_pool::available_descriptors(vk::DescriptorType type) const {
	return std::accumulate(blocks_.begin(), blocks_.end(), 0u,
						   [type](uint32_t s, const simple_descriptor_pool& p) {
							   return s + p.available_descriptors(type);
						   });
}

uint32_t growing_simple_descriptor_pool::available_sets() const {
	return std::accumulate(
			blocks_.begin(), blocks_.end(), 0u,
			[](uint32_t s, const simple_descriptor_pool& p) { return s + p.available_sets(); });
}

descriptor_set growing_simple_descriptor_pool::allocate_descriptor_set(
		const std::shared_ptr<const descriptor_set_layout>& layout, bool store_layout) {
	descriptor_set_resources req = *layout;
	if(!block_resources_.sufficient_for(req)) {
		throw mce::graphics_exception("Insufficient resources per block for requested allocation.");
	}
	auto it = std::find_if(blocks_.begin(), blocks_.end(), [&req](const simple_descriptor_pool& blk) {
		return blk.available_resources().sufficient_for(req);
	});
	if(it != blocks_.end()) {
		return it->allocate_descriptor_set(layout, store_layout);
	} else {
		blocks_.emplace_back(*dev_, block_resources_);
		return blocks_.back().allocate_descriptor_set(layout, store_layout);
	}
}

std::vector<descriptor_set> growing_simple_descriptor_pool::allocate_descriptor_sets(
		const std::vector<std::shared_ptr<const descriptor_set_layout>>& layouts, bool store_layout) {
	descriptor_set_resources req;
	for(const auto& layout : layouts) {
		req += *layout;
	}
	if(!block_resources_.sufficient_for(req)) {
		throw mce::graphics_exception("Insufficient resources per block for requested allocation.");
	}
	auto it = std::find_if(blocks_.begin(), blocks_.end(), [&req](const simple_descriptor_pool& blk) {
		return blk.available_resources().sufficient_for(req);
	});
	if(it != blocks_.end()) {
		return it->allocate_descriptor_sets(layouts, store_layout);
	} else {
		blocks_.emplace_back(*dev_, block_resources_);
		return blocks_.back().allocate_descriptor_sets(layouts, store_layout);
	}
}

void growing_simple_descriptor_pool::reset() {
	for(auto& blk : blocks_) {
		blk.reset();
	}
}

void growing_simple_descriptor_pool::reset_and_shrink() {
	blocks_.erase(blocks_.begin() + 1, blocks_.end());
	blocks_.front().reset();
}

uint32_t growing_simple_descriptor_pool::descriptors_capacity(vk::DescriptorType type) const {
	return std::accumulate(
			blocks_.begin(), blocks_.end(), 0u,
			[type](uint32_t s, const simple_descriptor_pool& p) { return s + p.max_descriptors(type); });
}
uint32_t growing_simple_descriptor_pool::sets_capacity() const {
	return std::accumulate(blocks_.begin(), blocks_.end(), 0u,
						   [](uint32_t s, const simple_descriptor_pool& p) { return s + p.max_sets(); });
}

descriptor_set_resources growing_simple_descriptor_pool::available_resources() const {
	descriptor_set_resources rv;
	for(const auto& blk : blocks_) {
		rv += blk.available_resources();
	}
	return rv;
}
descriptor_set_resources growing_simple_descriptor_pool::resource_capacity() const {
	descriptor_set_resources rv;
	for(const auto& blk : blocks_) {
		rv += blk.max_resources();
	}
	return rv;
}

} /* namespace graphics */
} /* namespace mce */
