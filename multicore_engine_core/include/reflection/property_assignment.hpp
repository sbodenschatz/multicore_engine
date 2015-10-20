/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/property_assignment.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_ASSIGNMENT_HPP_
#define REFLECTION_PROPERTY_ASSIGNMENT_HPP_

#include <string>
#include "property.hpp"

namespace mce {
namespace reflection {

template <typename Root_Type>
class abstract_property;
template <typename Root_Type, typename T>
class property;

template <typename Root_Type>
class abstract_property_assignment {
protected:
	bool valid_;

public:
	abstract_property_assignment() noexcept : valid_(false) {}
	abstract_property_assignment(const abstract_property_assignment&) = delete;
	abstract_property_assignment(abstract_property_assignment&&) = delete;
	abstract_property_assignment& operator=(const abstract_property_assignment&) = delete;
	abstract_property_assignment& operator=(abstract_property_assignment&&) = delete;
	virtual ~abstract_property_assignment() = default;
	virtual void assign(Root_Type& object) const = 0;
	virtual void parse(const std::string& value_string) = 0;
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept = 0;

	bool valid() const {
		return valid_;
	}
};

template <typename Root_Type, typename T>
class property_assignment : public abstract_property_assignment<Root_Type> {
	const property<Root_Type, T>& property;
	T value;

public:
	property_assignment(const mce::reflection::property<Root_Type, T>& property) : property(property) {}
	property_assignment(const property_assignment&) = delete;
	property_assignment(property_assignment&&) = delete;
	property_assignment& operator=(const property_assignment&) = delete;
	property_assignment& operator=(property_assignment&&) = delete;
	virtual ~property_assignment() = default;

	virtual void assign(Root_Type& object) const override {
		if(this->valid_) property.set_value(object, value);
	}
	virtual void parse(const std::string& value_string) override {
		this->valid_ = type_parser<T>::from_string(value_string, value);
	}
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept override {
		return property;
	}
};

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_ASSIGNMENT_HPP_ */
