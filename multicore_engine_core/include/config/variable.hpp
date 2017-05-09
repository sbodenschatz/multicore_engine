/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_HPP_
#define CONFIG_VARIABLE_HPP_

#include <memory>
#include <reflection/property.hpp>
#include <string>
#include <util/traits.hpp>
#include <util/type_id.hpp>

namespace mce {
namespace config {
template <typename T>
class variable;
class config_store;

class abstract_variable : public std::enable_shared_from_this<abstract_variable> {
	std::string full_name_;
	util::type_id_t type_id_;

	friend class variable_group;
	void load_value_from_store(config_store& store);

protected:
	std::unique_ptr<reflection::abstract_property<abstract_variable>> property_;
	abstract_variable(const std::string& full_name, util::type_id_t type_id)
			: full_name_{full_name}, type_id_{type_id} {}

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

	reflection::abstract_property<abstract_variable>* property() const {
		return property_.get();
	}
};

template <typename T>
class variable : public abstract_variable {
	T value_;

public:
	explicit variable(const std::string& full_name)
			: abstract_variable(full_name, util::type_id<abstract_variable>::id<T>()) {
		property_ = reflection::make_property("value", &value_);
	}

	util::accessor_value_type_t<T> value() const {
		return value_;
	}

	void value(util::accessor_value_type_t<T> value) {
		value_ = value;
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
