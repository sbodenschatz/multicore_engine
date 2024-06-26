/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/reflection/property.hpp
 * Copyright 2015-2018 by Stefan Bodenschatz
 */

#ifndef REFLECTION_PROPERTY_HPP_
#define REFLECTION_PROPERTY_HPP_

/**
 * \file
 * Defines the representation of reflected properties of objects.
 */

#include <algorithm>
#include <boost/utility/string_view.hpp>
#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/exceptions.hpp>
#include <mce/reflection/type.hpp>
#include <mce/util/traits.hpp>
#include <mce/util/type_id.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace mce {
namespace reflection {

/// Implements the abstract base for an assignment class that does nothing.
/**
 * Is not actually abstract because there are no member functions that would normally be abstract here.
 */
template <typename U>
class abstract_null_assignment {};
template <typename U, typename V>
class null_assignment;

template <typename Root_Type, template <typename> class AbstractAssignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
class abstract_property;
template <typename Root_Type, typename T, template <typename> class AbstractAssignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
class property;

class property_type_id_tag;

/// Implements an assignment class that does nothing.
template <typename U, typename V>
class null_assignment : public abstract_null_assignment<U> {
public:
	/// Creates a null assignment for the given property and arbitrary parameters.
	template <typename... Args>
	null_assignment(const mce::reflection::property<U, V, mce::reflection::abstract_null_assignment,
													mce::reflection::null_assignment>&,
					const Args&...) {}
};

/// Represents a property of any Root_Type object regardless of concrete type of property and object.
template <typename Root_Type, template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class abstract_property {
protected:
	std::string name_;			   ///< Stores the name of the property.
	mce::reflection::type_t type_; ///< Stores the type representation of the property.
	mce::util::type_id_t type_id_; ///< Stores the type_id of the type of the property.

	/// Constructs an abstract_property with the given name, type and type_id.
	// cppcheck-suppress passedByValue
	explicit abstract_property(std::string name, mce::reflection::type_t type, util::type_id_t type_id)
			: name_(std::move(name)), type_(type), type_id_(type_id) {}

public:
	/// Forbids copy-construction.
	abstract_property(const abstract_property&) = delete;
	/// Forbids move-construction.
	abstract_property(abstract_property&&) = delete;
	/// Forbids copy-assignment.
	abstract_property& operator=(const abstract_property&) = delete;
	/// Forbids move-assignment.
	abstract_property& operator=(abstract_property&&) = delete;
	/// Allows polymorphic destruction.
	virtual ~abstract_property() = default;
	/// Returns a representation of the data type of the property.
	mce::reflection::type_t type() const noexcept {
		return type_;
	}
	/// Returns the type_id of the data type of the property.
	mce::util::type_id_t type_id() const noexcept {
		return type_id_;
	}
	/// Creates an assignment object for the concrete property value type from the given parameters.
	virtual std::unique_ptr<Abstract_Assignment<Root_Type>> make_assignment(Assignment_Param...) const = 0;
	/// Returns the name of the property given on construction of the property object.
	const std::string& name() const {
		return name_;
	}
	/// Parses a value from a string and assigns it to the property on the given object.
	virtual bool from_string(Root_Type& object, const boost::string_view& str) const = 0;
	/// Retrieves the property value from the given object, formats it to a string and returns it.
	virtual std::string to_string(const Root_Type& object) const = 0;
	/// Writes the value of the property on the given object to the given binary stream.
	virtual void from_bstream(Root_Type& object, bstream::ibstream& istr) const = 0;
	/// Reads the value of the property on the given object from the given binary stream.
	virtual void to_bstream(const Root_Type& object, bstream::obstream& ostr) const = 0;

	/// \brief Returns the abstract_property casted to a property of type T if it actually has the type T or
	/// nullptr otherwise.
	template <typename T>
	const property<Root_Type, T, Abstract_Assignment, Assignment, Assignment_Param...>* as_type() const {
		if(mce::util::type_id<property_type_id_tag>::id<T>() == type_id_) {
			return static_cast<
					const property<Root_Type, T, Abstract_Assignment, Assignment, Assignment_Param...>*>(
					this);
		} else {
			return nullptr;
		}
	}

	/// Returns true if the property is of the given data type T or false otherwise.
	template <typename T>
	bool is_type() const {
		return mce::util::type_id<property_type_id_tag>::id<T>() == type_id_;
	}
};

namespace detail {

template <typename T, typename Object_Type = void>
struct property_type_helper {
	typedef util::accessor_value_type_t<T> accessor_value;
	typedef T (Object_Type::*getter)() const;
	typedef void (Object_Type::*setter)(accessor_value value);
};

template <typename T>
struct property_type_helper<T, void> {
	typedef util::accessor_value_type_t<T> accessor_value;
};

template <typename T, typename known_type>
struct property_type_parser_helper {
	static auto from_string(const boost::string_view& s) {
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
	static std::pair<T, bool> from_string(const boost::string_view&) {
		throw invalid_property_access_exception("Parsing unknown values from strings is not supported.");
	}
	static std::string to_string(const T&) {
		throw invalid_property_access_exception("Formating unknown values to strings is not supported.");
	}
};

} // namespace detail

/// Represents a property of any Root_Type object with the value type T regardless of concrete object type.
/**
 * Beware that for non-fundamental types T bound getters must be able to return the value by const reference,
 * therefore they are not allowed to return locals or temporaries. This requirement is added a a trade of to
 * not require copying of complex types. Because the type is specified in the virtual interface function that
 * only depends on T it can't be changed by the linked property.
 */
template <typename Root_Type, typename T,
		  template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class property : public abstract_property<Root_Type, Abstract_Assignment, Assignment, Assignment_Param...> {
protected:
	/// \brief Constructs a property with the given name and a representation of the data type of the property
	/// based on the template parameter T.
	explicit property(const std::string& name)
			: abstract_property<Root_Type, Abstract_Assignment, Assignment, Assignment_Param...>(
					  name, type_info<T>::type, mce::util::type_id<property_type_id_tag>::id<T>()) {}

public:
	static_assert(std::is_base_of<Abstract_Assignment<Root_Type>, Assignment<Root_Type, T>>::value,
				  "The Abstract_Assignment template class has to be a base of the Assignment_Class");
	/// Defines the type of getter return value and setter value parameter as either T or const T&.
	typedef typename detail::property_type_helper<T, void>::accessor_value accessor_value;
	/// Forbids copy-construction.
	property(const property&) = delete;
	/// Forbids move-construction.
	property(property&&) = delete;
	/// Forbids copy-assignment.
	property& operator=(const property&) = delete;
	/// Forbids move-assignment.
	property& operator=(property&&) = delete;
	/// Allows polymorphic destruction.
	virtual ~property() = default;
	/// Provides read access to the property value for the given object.
	/**
	 * The return value type is T for all types to allow thread-safe getters.
	 */
	virtual T get_value(const Root_Type& object) const = 0;
	/// Provides write access to the property value for the given object.
	/**
	 * The value parameter type is T for primitive types and const T& for complex types (strings, vecN, etc.).
	 */
	virtual void set_value(Root_Type& object, accessor_value value) const = 0;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif
	/// Parses a value from a string and assigns it to the property on the given object.
	virtual bool from_string(Root_Type& object, const boost::string_view& str) const override {
		using helper = detail::property_type_parser_helper<T, typename type_info<T>::known_type>;
		auto r = helper::from_string(str);
		if(r.second) set_value(object, r.first);
		return r.second;
	}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	/// Retrieves the property value from the given object, formats it to a string and returns it.
	virtual std::string to_string(const Root_Type& object) const override {
		using helper = detail::property_type_parser_helper<T, typename type_info<T>::known_type>;
		return helper::to_string(get_value(object));
	}
	/// Creates an assignment object for T from the given parameters.
	virtual std::unique_ptr<Abstract_Assignment<Root_Type>>
	make_assignment(Assignment_Param...) const override;
	/// Writes the value of the property on the given object to the given binary stream.
	virtual void from_bstream(Root_Type& object, bstream::ibstream& istr) const override {
		T val; // TODO: Implement type-specific override for initialization of types with uninitialized
			   // members (e.g. glm types)
		istr >> val;
		set_value(object, val);
	}
	/// Reads the value of the property on the given object from the given binary stream.
	virtual void to_bstream(const Root_Type& object, bstream::obstream& ostr) const override {
		accessor_value val = get_value(object);
		ostr << val;
	}
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4505)
#endif
///@cond DOXYGEN_IGNORE
template <typename Root_Type, typename T, template <typename> class Abstract_Assignment,
		  template <typename, typename> class Assignment, typename... Assignment_Param>
std::unique_ptr<Abstract_Assignment<Root_Type>>
property<Root_Type, T, Abstract_Assignment, Assignment, Assignment_Param...>::make_assignment(
		Assignment_Param... param) const {
	return std::make_unique<Assignment<Root_Type, T>>(*this, param...);
}
///@endcond
#ifdef _MSC_VER
#pragma warning(pop)
#endif

/// \brief Represents a property of a Root_Type object with a type of T bound to a concrete object type using
/// a getter and setter member function pointer on class Object_Type.
/**
 * Beware that for non-fundamental types T bound getters must be able to return the value by const reference,
 * therefore they are not allowed to return locals or temporaries. This requirement is added a a trade of to
 * not require copying of complex types. Because the type is specified in the virtual interface function that
 * only depends on T it can't be changed by the linked property.
 */
template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class AbstractAssignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class linked_property final
		: public property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...> {
public:
	/// Defines the type of getter return value and setter value parameter as either T or const T&.
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	/// Defines the member function pointer type for the getter.
	typedef typename detail::property_type_helper<T, Object_Type>::getter getter_t;
	/// Defines the member function pointer type for the setter.
	typedef typename detail::property_type_helper<T, Object_Type>::setter setter_t;

private:
	getter_t getter;
	setter_t setter;

public:
	/// Constructs a linked_property with the given name and using the given getter and setter.
	/**
	 * The getter and setter are allowed to be nullptr to disable read or write access.
	 */
	linked_property(const std::string& name, getter_t getter, setter_t setter)
			: property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...>(name),
			  getter(getter), setter(setter) {}
	/// Forbids copy-construction.
	linked_property(const linked_property&) = delete;
	/// Forbids move-construction.
	linked_property(linked_property&&) = delete;
	/// Forbids copy-assignment.
	linked_property& operator=(const linked_property&) = delete;
	/// Forbids move-assignment.
	linked_property& operator=(linked_property&&) = delete;
	/// Allows polymorphic destruction.
	virtual ~linked_property() = default;
	/// Provides read access to the property value for the given object.
	/**
	 * The return value type is T for all types to allow thread-safe getters.
	 *
	 * If no getter was supplied on construction of the property, this member function will throw an exception
	 * of type invalid_property_access_exception.
	 */
	virtual T get_value(const Root_Type& object) const override {
		if(getter)
			return (static_cast<const Object_Type&>(object).*getter)();
		else
			throw invalid_property_access_exception("Attempt to get not readable property.");
	}
	/// Provides write access to the property value for the given object.
	/**
	 * The value parameter type is T for primitive types and const T& for complex types (strings, vecN, etc.).
	 *
	 * If no setter was supplied on construction of the property, this member function will throw an exception
	 * of type invalid_property_access_exception.
	 */
	virtual void set_value(Root_Type& object, accessor_value value) const override {
		if(setter)
			(static_cast<Object_Type&>(object).*setter)(value);
		else
			throw invalid_property_access_exception("Attempt to set not writable property.");
	}
};

/// \brief Represents a property of a Root_Type object with a type of T bound to a concrete object type using
/// a member variable pointer on class Object_Type.
template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class AbstractAssignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
class directly_linked_property final
		: public property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...> {
public:
	/// Defines the type of getter return value and setter value parameter as either T or const T&.
	typedef typename detail::property_type_helper<T, Object_Type>::accessor_value accessor_value;
	/// Defines the member variable pointer type for the bound variable.
	typedef T Object_Type::*variable_t;

private:
	variable_t variable;
	bool read_only = false;

public:
	/// \brief Constructs a directly_linked_property with the given name that is bound to the member variable
	/// to which a member pointer is given.
	/**
	 * Read access can be disabled by supplying true to the read_only parameter.
	 */
	directly_linked_property(const std::string& name, variable_t variable, bool read_only)
			: property<Root_Type, T, AbstractAssignment, Assignment, Assignment_Param...>(name),
			  variable(variable), read_only(read_only) {}
	/// Forbids copy-construction.
	directly_linked_property(const directly_linked_property&) = delete;
	/// Forbids move-construction.
	directly_linked_property(directly_linked_property&&) = delete;
	/// Forbids copy-assignment.
	directly_linked_property& operator=(const directly_linked_property&) = delete;
	/// Forbids move-assignment.
	directly_linked_property& operator=(directly_linked_property&&) = delete;
	/// Allows polymorphic destruction.
	virtual ~directly_linked_property() = default;
	/// Provides read access to the property value for the given object.
	/**
	 * The return value type is T for all types to allow thread-safe getters.
	 */
	virtual T get_value(const Root_Type& object) const noexcept override {
		return static_cast<const Object_Type&>(object).*variable;
	}
	/// Provides write access to the property value for the given object.
	/**
	 * The value parameter type is T for primitive types and const T& for complex types (strings, vecN, etc.).
	 *
	 * If read_only was true on construction of the property, this member function will throw an exception of
	 * type invalid_property_access_exception.
	 */
	virtual void set_value(Root_Type& object, accessor_value value) const override {
		if(read_only)
			throw invalid_property_access_exception("Attempt to set not writable property.");
		else
			static_cast<Object_Type&>(object).*variable = value;
	}
};

/// Creates a property with the given name and using the given getter and setter.
/**
 * Creates a linked_property object.
 *
 * The getter and setter are allowed to be nullptr to disable read or write access.
 */
template <typename Root_Type, typename T, typename Object_Type,
		  template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param>
std::unique_ptr<abstract_property<Root_Type, Abstract_Assignment, Assignment, Assignment_Param...>>
make_property(const std::string& name, typename detail::property_type_helper<T, Object_Type>::getter getter,
			  typename detail::property_type_helper<T, Object_Type>::setter setter = nullptr) {
	return std::make_unique<
			linked_property<Root_Type, T, Object_Type, Abstract_Assignment, Assignment, Assignment_Param...>>(
			name, getter, setter);
}

/// \brief Creates a property with the given name that is bound to the member variable to which a member
/// pointer is given.
/**
 * Creates a directly_linked_property object.
 *
 * Read access can be disabled by supplying true to the read_only parameter.
 */
template <typename Root_Type, template <typename> class Abstract_Assignment = abstract_null_assignment,
		  template <typename, typename> class Assignment = null_assignment, typename... Assignment_Param,
		  typename T, typename Object_Type>
std::unique_ptr<abstract_property<Root_Type, Abstract_Assignment, Assignment, Assignment_Param...>>
make_property(const std::string& name, T Object_Type::*variable, bool read_only = false) {
	return std::make_unique<directly_linked_property<Root_Type, T, Object_Type, Abstract_Assignment,
													 Assignment, Assignment_Param...>>(name, variable,
																					   read_only);
}

} // namespace reflection
} // namespace mce

#endif /* REFLECTION_PROPERTY_HPP_ */
