/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/config/config_store.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/flat_set.hpp>
#include <config/config_store.hpp>

namespace mce {
namespace config {

void config_store::store(std::ostream& user_config) {
	to_string_data();
	for(const auto& key : key_order) {
		auto it = config_file_data_.find(key);
		if(it != config_file_data_.end()) {
			user_config << it->first << "=" << it->second;
		} else {
			user_config << "\n";
		}
	}
}
void config_store::read_config_file_data(std::istream& input) {
	static_cast<void>(input);
}
void config_store::parse_data() {}
void config_store::to_string_data() {
	// This function is only allowed to be called in single-threaded context, therefore we can use a local
	// static for temp data.
	static boost::container::flat_set<boost::string_view> user_defined_variables;
	user_defined_variables.clear();
	static std::vector<std::string> added_variables;
	added_variables.clear();
	std::copy(key_order.begin(), key_order.end(),
			  std::inserter(user_defined_variables, user_defined_variables.begin()));
	for(const auto& var : variables_) {
		if(var.second->dirty() && user_defined_variables.count(var.first) < 1) {
			added_variables.push_back(var.first);
		}
	}
	auto old_user_var_count = std::distance(key_order.begin(), key_order.end());
	std::move(added_variables.begin(), added_variables.end(), std::back_inserter(key_order));
	std::copy(key_order.begin() + old_user_var_count, key_order.end(),
			  std::inserter(user_defined_variables, user_defined_variables.begin()));
	added_variables.clear();
	for(const auto& var : variables_) {
		config_file_data_[var.first] = var.second->format_value_to_string();
	}
	user_defined_variables.clear();
}
void config_store::load(std::istream& user_config, std::istream& default_config) {
	static_cast<void>(user_config);
	static_cast<void>(default_config);
}

void config_store::save() {
	std::lock_guard<std::mutex> lock(config_mutex);
	config_storer s(*this);
	save_callback_(s);
}

config_store::~config_store() noexcept {
	config_storer s(*this);
	save_callback_(s);
}

} // namespace config
} // namespace mce
