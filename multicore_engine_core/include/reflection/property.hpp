/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/property.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_HPP_
#define REFLECTION_PROPERTY_HPP_

#include <memory>
#include <stdexcept>
#include <string>
#include "type.hpp"

namespace mce {
namespace reflection {

template <typename Root_Type>
class abstract_property_assignment;
template <typename Root_Type, typename T>
class property_assignment;

template <typename Root_Type>
class abstract_property {
protected:
	std::string name_;

public:
	abstract_property(const std::string& name) : name_(name) {}
	abstract_property(const abstract_property&) = delete;
	abstract_property(abstract_property&&) = delete;
	abstract_property& operator=(const abstract_property&) = delete;
	abstract_property& operator=(abstract_property&&) = delete;
	virtual ~abstract_property() = default;
	virtual mce::reflection::type_t type() const noexcept = 0;
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_assignment() const = 0;
	const std::string& name() const {
		return name_;
	}
	// TODO: Implement interface for binary serialization of objects
};

namespace detail {

template <typename T, typename Object_Type = void>
struct property_type_helper {
	typedef typename std::conditional<std::is_fundamental<T>::value, T, const T&>::type accessor_value;
	typedef accessor_value (Object_Type::*getter)() const;
	typedef void (Object_Type::*setter)(accessor_value value);
};

template <typename T>
struct property_type_helper<T, void> {
	typedef typename std::conditional<std::is_fundamental<T>::value, T, const T&>::type accessor_value;
};

} // namespace detail

template <typename Root_Type, typename T>
class property : public abstract_property<Root_Type> {
public:
	typedef typename detail::property_type_helper<T, void>::accessor_value accessor_value;
	property(const std::string& name) : abstract_property<Root_Type>(name) {}
	property(const property&) = delete;
	property(property&&) = delete;
	property& operator=(const property&) = delete;
	property& operator=(property&&) = delete;
	virtual ~property() = default;
	virtual mce::reflection::type_t type() const noexcept override {
		return type_info<T>::type;
	}
	virtual accessor_value get_value(const Root_Type& object) const = 0;
	virtual void set_value(Root_Type& object, accessor_value value) const = 0;
	virtual std::unique_ptr<abstract_property_assignment<Root_Type>> make_assignment() const override;
};

} // namespace reflection
} // namespace mce
#include "property_assignment.hpp"
namespace mce {
namespace reflection {

template <typename Root_Type, typename T>
std::unique_ptr<abstract_property_assignment<Root_Type>> property<Root_Type, T>::make_assignment() const {
	return std::make_unique<mce::reflection::property_assignment<Root_Type, T>>(*this);
}

template <typename Root_Type, typename T, typename Object_Type>
class linked_property : public property<Root_Type, T> {
public:
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	typedef typename detail::property_type_helper<T, Object_Type>::getter getter_t;
	typedef typename detail::property_type_helper<T, Object_Type>::setter setter_t;

private:
	getter_t getter;
	setter_t setter;

public:
	linked_property(const std::string& name, getter_t getter, setter_t setter)
			: property<Root_Type, T>(name), getter(getter), setter(setter) {}
	linked_property(const linked_property&) = delete;
	linked_property(linked_property&&) = delete;
	linked_property& operator=(const linked_property&) = delete;
	linked_property& operator=(linked_property&&) = delete;
	virtual ~linked_property() = default;
	virtual accessor_value get_value(const Root_Type& object) const override {
		if(getter)
			return (static_cast<const Object_Type&>(object).*getter)();
		else
			throw std::logic_error("Attempt to get not readable property.");
	}
	virtual void set_value(Root_Type& object, accessor_value value) const override {
		if(setter)
			(static_cast<Object_Type&>(object).*setter)(value);
		else
			throw std::logic_error("Attempt to set not writable property.");
	}
};

template <typename Root_Type, typename T, typename Object_Type>
class directly_linked_property : public property<Root_Type, T> {
public:
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	typedef T Object_Type::*variable_t;

private:
	variable_t variable;
	bool read_only = false;

public:
	directly_linked_property(const std::string& name, variable_t variable, bool read_only)
			: property<Root_Type, T>(name), variable(variable), read_only(read_only) {}
	directly_linked_property(const directly_linked_property&) = delete;
	directly_linked_property(directly_linked_property&&) = delete;
	directly_linked_property& operator=(const directly_linked_property&) = delete;
	directly_linked_property& operator=(directly_linked_property&&) = delete;
	virtual ~directly_linked_property() = default;
	virtual accessor_value get_value(const Root_Type& object) const noexcept override {
		return static_cast<const Object_Type&>(object).*variable;
	}
	virtual void set_value(Root_Type& object, accessor_value value) const override {
		if(read_only)
			throw std::logic_error("Attempt to set not writable property.");
		else
			static_cast<Object_Type&>(object).*variable = value;
	}
};

template <typename Root_Type, typename T, typename Object_Type>
std::unique_ptr<abstract_property<Root_Type>>
make_property(const std::string& name, typename detail::property_type_helper<T, Object_Type>::getter getter,
			  typename detail::property_type_helper<T, Object_Type>::setter setter = nullptr) {
	return std::make_unique<linked_property<Root_Type, T, Object_Type>>(name, getter, setter);
}

template <typename Root_Type, typename T, typename Object_Type>
std::unique_ptr<abstract_property<Root_Type>> make_property(const std::string& name, T Object_Type::*variable,
															bool read_only = false) {
	return std::make_unique<directly_linked_property<Root_Type, T, Object_Type>>(name, variable, read_only);
}

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_HPP_ */
