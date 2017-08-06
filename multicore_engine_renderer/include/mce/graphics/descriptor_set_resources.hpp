/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/descriptor_pool_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_DESCRIPTOR_SET_RESOURCES_HPP_
#define MCE_GRAPHICS_DESCRIPTOR_SET_RESOURCES_HPP_

/**
 * \file
 * Defines the descriptor_set_resources class to represent resource amounts related to descriptors.
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <boost/operators.hpp>
#include <mce/containers/generic_flat_map.hpp>
#include <mce/graphics/descriptor_set_layout.hpp>

namespace mce {
namespace graphics {

/// \brief Represents resource amounts associated with descriptor sets, layouts and pools consisting of
/// descriptor counts per descriptor types and a number of descriptor sets.
class descriptor_set_resources
		: public boost::equality_comparable<
				  descriptor_set_resources,
				  boost::additive<descriptor_set_resources,
								  boost::multipliable<descriptor_set_resources, uint32_t>>> {
	template <typename T>
	using sized_small_vector = boost::container::small_vector<T, 16>;

	containers::generic_flat_map<sized_small_vector, vk::DescriptorType, uint32_t> descriptors_;
	uint32_t descriptor_sets_;

public:
	/// Constructs an empty descriptor_set_resources object.
	descriptor_set_resources() : descriptor_sets_{0} {}

	/// Constructs a descriptor_set_resources object with the given descriptor and descriptor set counts.
	explicit descriptor_set_resources(vk::ArrayProxy<const vk::DescriptorPoolSize> descriptors,
									  uint32_t descriptor_sets = 1)
			: descriptor_sets_{descriptor_sets} {
		for(const auto& d : descriptors) {
			descriptors_[d.type] += d.descriptorCount;
		}
	}

	/// \brief Constructs a descriptor_set_resources object with the amounts required for the given number of
	/// descriptor sets with the given layout.
	// cppcheck-suppress noExplicitConstructor
	descriptor_set_resources(const descriptor_set_layout& layout, uint32_t descriptor_sets = 1)
			: descriptor_sets_{descriptor_sets} {
		for(const auto& d : layout.bindings()) {
			descriptors_[d.descriptor_type] += descriptor_sets * d.descriptor_count;
		}
	}

	/// Allows copy-construction.
	descriptor_set_resources(const descriptor_set_resources&) = default;
	/// Allows copy-assignment.
	descriptor_set_resources& operator=(const descriptor_set_resources&) = default;
	/// Allows move-construction.
	/**
	 * The moved from object is left in a defined empty state as by default-construction.
	 */
	descriptor_set_resources(descriptor_set_resources&& other) noexcept
			: descriptors_{std::move(other.descriptors_)}, descriptor_sets_{other.descriptor_sets_} {
		other.descriptor_sets_ = 0;
		other.descriptors_ = decltype(other.descriptors_)();
	}
	/// Allows move-assignment.
	/**
	 * The moved from object is left in a defined empty state as by default-construction.
	 */
	descriptor_set_resources& operator=(descriptor_set_resources&& other) noexcept {
		descriptors_ = std::move(other.descriptors_);
		descriptor_sets_ = other.descriptor_sets_;
		other.descriptors_ = decltype(other.descriptors_)();
		other.descriptor_sets_ = 0;
		return *this;
	}

	/// Compares *this and other for equality.
	/**
	 * Boost.Operators provides operator!= based on this operator.
	 */
	bool operator==(const descriptor_set_resources& other) const {
		return std::tie(descriptors_, descriptor_sets_) ==
			   std::tie(other.descriptors_, other.descriptor_sets_);
	}

	/// Adds the resources of other to the resource amounts in this object.
	/**
	 * Boost.Operators provides operator+ based on this operator.
	 */
	descriptor_set_resources& operator+=(const descriptor_set_resources& other) {
		for(const auto& od : other.descriptors_) {
			descriptors_[od.first] += od.second;
		}
		descriptor_sets_ += other.descriptor_sets_;
		return *this;
	}
	/// Subtracts the resources in other from the resource amount in this object.
	/**
	 * Boost.Operators provides operator- based on this operator.
	 *
	 * Underflows of resource amounts are prevented by throwing a mce::out_of_range_exception if the operation
	 * would cause an underflow. In this case the resource amounts of this object are not changed.
	 */
	descriptor_set_resources& operator-=(const descriptor_set_resources& other) {
		if(descriptor_sets_ < other.descriptor_sets_) {
			throw mce::out_of_range_exception("Descriptor amounts would underflow.");
		}
		for(const auto& od : other.descriptors_) {
			auto it = descriptors_.find(od.first);
			if(it == descriptors_.end() || it->second < od.second) {
				throw mce::out_of_range_exception("Descriptor amounts would underflow.");
			}
		}
		for(const auto& od : other.descriptors_) {
			descriptors_.at(od.first) -= od.second;
		}
		descriptor_sets_ -= other.descriptor_sets_;
		return *this;
	}

	/// Multiplies the resource amounts in the this object by factor.
	/**
	 * Boost.Operators provides operator*(descriptor_set_resources,uint32_t) and
	 * operator*(uint32_t,descriptor_set_resources) based on this operator.
	 */
	descriptor_set_resources& operator*=(uint32_t factor) {
		for(auto& d : descriptors_) {
			d.second *= factor;
		}
		descriptor_sets_ *= factor;
		return *this;
	}

	/// Returns the smallest present resource amount in this object.
	uint32_t min_resource() const {
		if(descriptors_.empty()) return descriptor_sets_;
		return std::min(
				descriptor_sets_,
				std::min_element(descriptors_.begin(), descriptors_.end(), [](const auto& a, const auto& b) {
					return a.second < b.second;
				})->second);
	}

	/// Returns the number of descriptor sets.
	uint32_t descriptor_sets() const {
		return descriptor_sets_;
	}

	/// Allow access to the map for the number of descriptors for each descriptor type.
	const containers::generic_flat_map<sized_small_vector, vk::DescriptorType, uint32_t>&
	descriptors() const {
		return descriptors_;
	}

	/// Returns the number of descriptors for the given type.
	uint32_t descriptors(vk::DescriptorType type) const {
		auto it = descriptors_.find(type);
		if(it == descriptors_.end()) return 0;
		return it->second;
	}

	/// \brief Tests whether the resources listed in this object are sufficient for the given requested amount
	/// of resources.
	/**
	 * Essentially a and-combined component-wise >= comparison.
	 */
	bool sufficient_for(const descriptor_set_resources& request) const {
		if(descriptor_sets_ < request.descriptor_sets_) return false;
		for(const auto& rd : request.descriptors_) {
			if(descriptors(rd.first) < rd.second) return false;
		}
		return true;
	}
};

} // namespace graphics
} // namespace mce

#endif /* MCE_GRAPHICS_DESCRIPTOR_SET_RESOURCES_HPP_ */
