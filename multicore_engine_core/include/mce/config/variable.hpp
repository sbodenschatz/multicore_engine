/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/config/variable.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_HPP_
#define CONFIG_VARIABLE_HPP_

#include <boost/utility/string_view.hpp>
#include <cassert>
#include <mce/reflection/property.hpp>
#include <mce/util/local_function.hpp>
#include <mce/util/traits.hpp>
#include <mce/util/type_id.hpp>
#include <memory>
#include <mutex>
#include <string>

/**
 * \file
 * Defines the classes that represent configuration variables.
 */

namespace mce {
namespace config {
template <typename T>
class variable;

/// Provides the base class for variable objects of any type.
class abstract_variable : public std::enable_shared_from_this<abstract_variable> {
	std::string name_;
	util::type_id_t type_id_;

protected:
	/// Flag indicating, if the variable was modified.
	std::atomic<bool> dirty_;
	/// Property used to access variable value for user side access.
	std::unique_ptr<reflection::abstract_property<abstract_variable>> property_;
	/// Property used to access variable value for the config_store (resets dirty flag).
	std::unique_ptr<reflection::abstract_property<abstract_variable>> property_for_store_;

	/// Allows construction of variable objects by subclasses.
	abstract_variable(const std::string& name, util::type_id_t type_id)
			: name_{name}, type_id_{type_id}, dirty_{false} {}

	friend class config_store;
	/// Provides access to property_for_store_.
	const reflection::abstract_property<abstract_variable>* property_for_store() const {
		return property_for_store_.get();
	}
	/// Parses the variable value from the given string using property_for_store_.
	bool parse_value_from_string_from_store(const boost::string_view& value_string) {
		assert(property_);
		return property_for_store_->from_string(*this, value_string);
	}

public:
	/// \brief Attempts to downcast the variable to the given type U, or returns an empty shared_ptr if the
	/// variable is not of type U.
	template <typename U>
	std::shared_ptr<variable<U>> as_type();
	/// Checks if the variable is of type U.
	template <typename U>
	bool is_type() {
		return util::type_id<abstract_variable>::id<U>() == type_id_;
	}

	/// Returns the name of the variable.
	const std::string& name() const {
		return name_;
	}

	/// Provides access to a property object linked to the variables value.
	const reflection::abstract_property<abstract_variable>* property() const {
		return property_.get();
	}

	/// Parses the value of the variable from the given string.
	bool parse_value_from_string(const boost::string_view& value_string) {
		assert(property_);
		return property_->from_string(*this, value_string);
	}

	/// Returns a string representation of the value of the variable.
	std::string format_value_to_string() const {
		return property_->to_string(*this);
	}

	/// Returns a bool indicating, if the variable was modified.
	bool dirty() const {
		return dirty_;
	}
};

/// Implements a configuration variable of type T.
template <typename T>
class variable : public abstract_variable {
public:
	/// Handle type used to identify listeners.
	typedef size_t listener_handle;

private:
	T value_;
	mutable std::mutex mutex_;
	struct construction_key_token {};
	friend class config_store;

	void value_from_store(util::accessor_value_type_t<T> value) {
		std::unique_lock<std::mutex> lock(mutex_);
		value_ = value;
		dirty_ = false;
		if(!modification_listeners.empty()) {
			lock.unlock();
			for(auto& listener : modification_listeners) {
				listener.second(value);
			}
		}
	}

	listener_handle next_listener_id = 0;
	std::vector<std::pair<listener_handle, util::local_function<128, void(const T&)>>> modification_listeners;

public:
	/// Allows construction of variable objects by the config_store.
	/**
	 * Usage by other code is prevented by requiring a construction_key_token that can only be constructed by
	 * friend classes instead of making the constructor private because variables are created using
	 * std::make_shared.
	 */
	explicit variable(const std::string& full_name, construction_key_token)
			: abstract_variable(full_name, util::type_id<abstract_variable>::id<T>()) {
		property_ = reflection::make_property<abstract_variable, T, variable<T>>(
				"value", static_cast<T (variable<T>::*)() const>(&variable<T>::value),
				static_cast<void (variable<T>::*)(util::accessor_value_type_t<T>)>(&variable<T>::value));
		property_for_store_ = reflection::make_property<abstract_variable, T, variable<T>>(
				"value_from_store", static_cast<T (variable<T>::*)() const>(&variable<T>::value),
				static_cast<void (variable<T>::*)(util::accessor_value_type_t<T>)>(
						&variable<T>::value_from_store));
	}

	/// Returns the value of the variable.
	T value() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return value_;
	}

	/// Sets the value of the variable to the given new value and notifies modification listeners.
	void value(util::accessor_value_type_t<T> value) {
		std::unique_lock<std::mutex> lock(mutex_);
		value_ = value;
		dirty_ = true;
		if(!modification_listeners.empty()) {
			lock.unlock();
			for(auto& listener : modification_listeners) {
				listener.second(value);
			}
		}
	}

	/// \brief Calls the given transaction function object with a modifiable reference to to the variable
	/// value under mutex and notifies the modification listeners (after dropping the mutex).
	template <typename F>
	void do_transaction(F&& transaction) {
		std::unique_lock<std::mutex> lock(mutex_);
		transaction(value_);
		if(!modification_listeners.empty()) {
			T value = value_;
			lock.unlock();
			for(auto& listener : modification_listeners) {
				listener.second(value);
			}
		}
	}

	/// \brief Adds a function object callable for <code>void(const T&)</code> as a modification listener to
	/// be called when the config variable value has changed.
	template <typename F>
	listener_handle add_modification_listener(F&& listener) {
		std::lock_guard<std::mutex> lock(mutex_);
		auto id = next_listener_id++;
		modification_listeners.emplace_back(id, std::forward<F>(listener));
		return id;
	}

	/// Removes the modification listener whose addition returned the given listener_handle.
	void remove_modification_listener(listener_handle id) {
		std::lock_guard<std::mutex> lock(mutex_);
		modification_listeners.erase(std::remove_if(modification_listeners.begin(),
													modification_listeners.end(),
													[id](auto& e) { return e.first == id; }),
									 modification_listeners.end());
	}
};

template <typename U>
std::shared_ptr<variable<U>> abstract_variable::as_type() {
	if(is_type<U>()) {
		return std::static_pointer_cast<variable<U>>(shared_from_this());
	} else {
		return std::shared_ptr<variable<U>>();
	}
}

} // namespace config
} // namespace mce

#endif /* CONFIG_VARIABLE_HPP_ */
