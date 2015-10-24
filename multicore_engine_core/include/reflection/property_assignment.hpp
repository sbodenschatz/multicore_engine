/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/property_assignment.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_ASSIGNMENT_HPP_
#define REFLECTION_PROPERTY_ASSIGNMENT_HPP_

#include <memory>
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
	abstract_property_assignment(const abstract_property_assignment&) = default;
	abstract_property_assignment(abstract_property_assignment&&) = default;
	abstract_property_assignment& operator=(const abstract_property_assignment&) = default;
	abstract_property_assignment& operator=(abstract_property_assignment&&) = default;
	virtual ~abstract_property_assignment() = default;
	virtual void assign(Root_Type& object) const = 0;
	virtual void parse(const std::string& value_string) = 0; // TODO Change to fit AST-Nodes
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept = 0;
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_copy() const = 0;
	// TODO: Implement interface for binary serialization of object configurations

	bool valid() const {
		return valid_;
	}
};

template <typename Root_Type, typename T>
class property_assignment : public abstract_property_assignment<Root_Type> {
	const property<Root_Type, T>& property_;
	T value;

public:
	property_assignment(const mce::reflection::property<Root_Type, T>& property) : property_(property) {}
	property_assignment(const property_assignment&) = default;
	property_assignment(property_assignment&&) = default;
	property_assignment& operator=(const property_assignment&) = default;
	property_assignment& operator=(property_assignment&&) = default;
	virtual ~property_assignment() = default;

	virtual void assign(Root_Type& object) const override {
		if(this->valid_) property_.set_value(object, value);
	}
	virtual void parse(const std::string& value_string) override {
		this->valid_ = type_parser<T>::from_string(value_string, value);
	}
	virtual const mce::reflection::abstract_property<Root_Type>& abstract_property() noexcept override {
		return property_;
	}
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_copy() const override {
		return std::make_unique<property_assignment<Root_Type, T>>(*this);
	}
};

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_ASSIGNMENT_HPP_ */
