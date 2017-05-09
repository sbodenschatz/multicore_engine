/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_HPP_
#define CONFIG_VARIABLE_HPP_

#include <string>
#include <util/type_id.hpp>

namespace mce {
namespace config {

class abstract_variable {
	std::string name_;
	util::type_id_t type_id_;

protected:
	abstract_variable(const std::string& name, util::type_id_t type_id) : name_{name}, type_id_{type_id} {}
};

template <typename T>
class variable_impl : public abstract_variable {
public:
	explicit variable_impl(const std::string& name)
			: abstract_variable(name, util::type_id<abstract_variable>::id<T>()) {}
};

} // namespace config
} // namespace mce

#endif /* CONFIG_VARIABLE_HPP_ */
