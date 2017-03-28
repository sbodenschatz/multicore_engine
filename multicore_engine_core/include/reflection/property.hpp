/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/reflection/property.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_HPP_
#define REFLECTION_PROPERTY_HPP_

#include "type.hpp"
#include <exceptions.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace mce {
namespace reflection {

template <typename U>
class abstract_null_assignment {};
template <typename U, typename V>
class null_assignment;

template <typename Root_Type, template <typename> class AbstractAssignment, typename... Assignment_Param>
class abstract_property;
template <typename Root_Type, typename T, template <typename> class AbstractAssignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
class property;

template <typename U, typename V>
class null_assignment : public abstract_null_assignment<U> {
public:
	template <typename... Args>
	null_assignment(const mce::reflection::property<U, V, mce::reflection::abstract_null_assignment,
													mce::reflection::null_assignment>&,
					const Args&...) {}
};

template <typename Root_Type, template <typename> class Abstract_Assignment = abstract_null_assignment,
		  typename... Assignment_Param>
class abstract_property {
protected:
	std::string name_;

	// cppcheck-suppress passedByValue
	explicit abstract_property(std::string name) : name_(std::move(name)) {}

public:
	/// Forbids copy-construction.
	abstract_property(const abstract_property&) = delete;
	abstract_property(abstract_property&&) = delete;
	abstract_property& operator=(const abstract_property&) = delete;
	abstract_property& operator=(abstract_property&&) = delete;
	virtual ~abstract_property() = default;
	virtual mce::reflection::type_t type() const noexcept = 0;
	virtual std::unique_ptr<Abstract_Assignment<Root_Type>> make_assignment(Assignment_Param...) const = 0;
	const std::string& name() const {
		return name_;
	}
	virtual bool from_string(Root_Type& object, const std::string& str) const = 0;
	virtual std::string to_string(const Root_Type& object) const = 0;
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

template <typename T, typename known_type>
struct property_type_parser_helper {
	static auto from_string(const std::string& s) {
		T t;
		auto res = type_parser<T>::from_string(s, t);
		return std::make_pair(t, res);
	}
	static std::string to_string(const T& t) {
		return type_parser<T>::to_string(t);
	}
};

template <typename T>
struct property_type_parser_helper<T, std::false_type> {
	static std::pair<T, bool> from_string(const std::string&) {
		throw invalid_property_access_exception("Parsing unknown values from strings is not supported.");
	}
	static std::string to_string(T&) {
		throw invalid_property_access_exception("Formating unknown values to strings is not supported.");
	}
};

} // namespace detail

template <typename Root_Type, typename T,
		  template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class property : public abstract_property<Root_Type, Abstract_Assignment, Assignment_Param...> {
protected:
	explicit property(const std::string& name)
			: abstract_property<Root_Type, Abstract_Assignment, Assignment_Param...>(name) {}

public:
	static_assert(std::is_base_of<Abstract_Assignment<Root_Type>, Assignment<Root_Type, T>>::value,
				  "The Abstract_Assignment template class has to be a base of the Assignment_Class");
	typedef typename detail::property_type_helper<T, void>::accessor_value accessor_value;
	/// Forbids copy-construction.
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
	virtual bool from_string(Root_Type& object, const std::string& str) const override {
		using helper = detail::property_type_parser_helper<T, typename type_info<T>::known_type>;
		auto r = helper::from_string(str);
		if(r.second) set_value(object, r.first);
		return r.second;
	}
	virtual std::string to_string(const Root_Type& object) const override {
		using helper = detail::property_type_parser_helper<T, typename type_info<T>::known_type>;
		return helper::to_string(get_value(object));
	}
	virtual std::unique_ptr<Abstract_Assignment<Root_Type>>
			make_assignment(Assignment_Param...) const override;
};

} // namespace reflection
} // namespace mce
#include <entity/component_property_assignment.hpp>
namespace mce {
namespace reflection {

template <typename Root_Type, typename T, template <typename> class Abstract_Assignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
std::unique_ptr<Abstract_Assignment<Root_Type>>
		property<Root_Type, T, Abstract_Assignment, Assignment, Assignment_Param...>::make_assignment(
				Assignment_Param... param) const {
	return std::make_unique<Assignment<Root_Type, T>>(*this, param...);
}

template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class AbstractAssignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class linked_property : public property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...> {
public:
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	typedef typename detail::property_type_helper<T, Object_Type>::getter getter_t;
	typedef typename detail::property_type_helper<T, Object_Type>::setter setter_t;

private:
	getter_t getter;
	setter_t setter;

public:
	linked_property(const std::string& name, getter_t getter, setter_t setter)
			: property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...>(name),
			  getter(getter), setter(setter) {}
	linked_property(const linked_property&) = delete;
	linked_property(linked_property&&) = delete;
	linked_property& operator=(const linked_property&) = delete;
	linked_property& operator=(linked_property&&) = delete;
	virtual ~linked_property() = default;
	virtual accessor_value get_value(const Root_Type& object) const override {
		if(getter)
			return (static_cast<const Object_Type&>(object).*getter)();
		else
			throw invalid_property_access_exception("Attempt to get not readable property.");
	}
	virtual void set_value(Root_Type& object, accessor_value value) const override {
		if(setter)
			(static_cast<Object_Type&>(object).*setter)(value);
		else
			throw invalid_property_access_exception("Attempt to set not writable property.");
	}
};

template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class AbstractAssignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class directly_linked_property
		: public property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...> {
public:
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	typedef T Object_Type::*variable_t;

private:
	variable_t variable;
	bool read_only = false;

public:
	directly_linked_property(const std::string& name, variable_t variable, bool read_only)
			: property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...>(name),
			  variable(variable), read_only(read_only) {}
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
			throw invalid_property_access_exception("Attempt to set not writable property.");
		else
			static_cast<Object_Type&>(object).*variable = value;
	}
};

template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
std::unique_ptr<abstract_property<Root_Type, Abstract_Assignment, Assignment_Param...>>
make_property(const std::string& name, typename detail::property_type_helper<T, Object_Type>::getter getter,
			  typename detail::property_type_helper<T, Object_Type>::setter setter = nullptr) {
	return std::make_unique<
			linked_property<Root_Type, T, Object_Type, Abstract_Assignment, Assignment, Assignment_Param...>>(
			name, getter, setter);
}

template <typename Root_Type, template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param,
		  typename T, typename Object_Type>
std::unique_ptr<abstract_property<Root_Type, Abstract_Assignment, Assignment_Param...>>
make_property(const std::string& name, T Object_Type::*variable, bool read_only = false) {
	return std::make_unique<directly_linked_property<Root_Type, T, Object_Type, Abstract_Assignment,
													 Assignment, Assignment_Param...>>(name, variable,
																					   read_only);
}

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_HPP_ */
