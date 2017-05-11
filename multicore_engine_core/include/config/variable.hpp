/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_HPP_
#define CONFIG_VARIABLE_HPP_

#include <boost/utility/string_view.hpp>
#include <cassert>
#include <memory>
#include <mutex>
#include <reflection/property.hpp>
#include <string>
#include <util/traits.hpp>
#include <util/type_id.hpp>

/**
 * \file
 * Defines the classes that represent configuration variables.
 */

namespace mce {
namespace config {
template <typename T>
class variable;

class abstract_variable : public std::enable_shared_from_this<abstract_variable> {
	std::string full_name_;
	util::type_id_t type_id_;

protected:
	std::atomic<bool> dirty_;
	std::unique_ptr<reflection::abstract_property<abstract_variable>> property_;
	std::unique_ptr<reflection::abstract_property<abstract_variable>> property_for_store_;

	abstract_variable(const std::string& full_name, util::type_id_t type_id)
			: full_name_{full_name}, type_id_{type_id}, dirty_{false} {}

	friend class config_store;
	const reflection::abstract_property<abstract_variable>* property_for_store() const {
		return property_for_store_.get();
	}
	bool parse_value_from_string_from_store(const boost::string_view& value_string) {
		assert(property_);
		return property_for_store_->from_string(*this, value_string);
	}

public:
	template <typename U>
	std::shared_ptr<variable<U>> as_type();
	template <typename U>
	bool is_type() {
		return util::type_id<abstract_variable>::id<U>() == type_id_;
	}

	const std::string& full_name() const {
		return full_name_;
	}

	const reflection::abstract_property<abstract_variable>* property() const {
		return property_.get();
	}

	bool parse_value_from_string(const boost::string_view& value_string) {
		assert(property_);
		return property_->from_string(*this, value_string);
	}
	std::string format_value_to_string() const {
		return property_->to_string(*this);
	}

	bool dirty() const {
		return dirty_;
	}
};

template <typename T>
class variable : public abstract_variable {
	T value_;
	mutable std::mutex value_mtx;
	struct construction_key_token {};
	friend class config_store;

	void value_from_store(util::accessor_value_type_t<T> value) {
		std::lock_guard<std::mutex> lock(value_mtx);
		value_ = value;
		dirty_ = false;
	}

public:
	explicit variable(const std::string& full_name, construction_key_token)
			: abstract_variable(full_name, util::type_id<abstract_variable>::id<T>()) {
		property_ = reflection::make_property<abstract_variable, T, variable<T>>(
				"value",
				static_cast<util::accessor_value_type_t<T>(variable<T>::*)() const>(&variable<T>::value),
				static_cast<void (variable<T>::*)(util::accessor_value_type_t<T>)>(&variable<T>::value));
		property_for_store_ = reflection::make_property<abstract_variable, T, variable<T>>(
				"value_from_store",
				static_cast<util::accessor_value_type_t<T>(variable<T>::*)() const>(&variable<T>::value),
				static_cast<void (variable<T>::*)(util::accessor_value_type_t<T>)>(
						&variable<T>::value_from_store));
	}

	util::accessor_value_type_t<T> value() const {
		std::lock_guard<std::mutex> lock(value_mtx);
		return value_;
	}

	void value(util::accessor_value_type_t<T> value) {
		std::lock_guard<std::mutex> lock(value_mtx);
		value_ = value;
		dirty_ = true;
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
