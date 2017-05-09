/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/variable_group.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_VARIABLE_GROUP_HPP_
#define CONFIG_VARIABLE_GROUP_HPP_

#include <boost/container/flat_map.hpp>
#include <config/variable.hpp>
#include <memory>
#include <string>

namespace mce {
namespace config {
class config_store;

class variable_group {
	config_store* store_;
	std::string full_name_;
	boost::container::flat_map<std::string, variable_group> subgroups_;
	boost::container::flat_map<std::string, std::shared_ptr<abstract_variable>> variables_;

public:
	variable_group(config_store& store, const std::string& full_name)
			: store_{&store}, full_name_{full_name} {}

	const boost::container::flat_map<std::string, variable_group>& subgroups() const {
		return subgroups_;
	}

	const boost::container::flat_map<std::string, std::shared_ptr<abstract_variable>>& variables() const {
		return variables_;
	}

	void create_subgroup(const std::string& name) {
		using namespace std::literals;
		subgroups_.emplace(name,
						   variable_group(*store_, (full_name_.empty() ? ""s : (full_name_ + "."s)) + name));
	}

	template <typename T>
	std::shared_ptr<variable<T>> create_variable(const std::string& name) {
		using namespace std::literals;
		auto var = std::make_shared<variable<T>>((full_name_.empty() ? ""s : (full_name_ + "."s)) + name);
		var->load_value_from_store(*store_);
		return var;
	}
};

} // namespace config
} // namespace mce

#endif /* CONFIG_VARIABLE_GROUP_HPP_ */
