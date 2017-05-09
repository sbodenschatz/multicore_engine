/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable_group.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_GROUP_HPP_
#define CONFIG_VARIABLE_GROUP_HPP_

#include <boost/container/flat_map.hpp>
#include <string>
#include <memory>

namespace mce {
namespace config {
class abstract_variable;

class variable_group {
	boost::container::flat_map<std::string, variable_group> subgroups_;
	boost::container::flat_map<std::string, std::shared_ptr<abstract_variable>> variables_;
};

} // namespace config
} // namespace mce

#endif /* CONFIG_VARIABLE_GROUP_HPP_ */
