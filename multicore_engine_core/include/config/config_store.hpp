/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/config/config_store.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONFIG_CONFIG_STORE_HPP_
#define CONFIG_CONFIG_STORE_HPP_

#include <boost/container/flat_map.hpp>
#include <config/variable.hpp>
#include <functional>
#include <istream>
#include <mutex>
#include <ostream>

namespace mce {
namespace config {

class config_store {
public:
	class config_storer {
		config_store& store_;
		explicit config_storer(config_store& owner) : store_{owner} {}

		friend class config_store;

	public:
		void store(std::ostream& user_config) {
			store_.store(user_config);
		}
	};

private:
	mutable std::mutex config_mutex;
	boost::container::flat_map<std::string, std::shared_ptr<abstract_variable>> variables_;
	boost::container::flat_map<std::string, std::string> config_file_data_;
	std::vector<std::string> key_order;
	std::function<void(config_storer&)> save_callback_;

	// Internal methods called under lock or during construction/destruction (i.e. single threaded):
	void store(std::ostream& user_config);
	void read_config_file_data(std::istream& input, bool user_config);
	void parse_data();
	void to_string_data();
	void load_internal(std::istream& user_config, std::istream& default_config);

public:
	template <typename F>
	config_store(std::istream& user_config, std::istream& default_config, F&& save_callback)
			: save_callback_{save_callback} {
		load_internal(user_config, default_config);
	}
	~config_store() noexcept;
	void save();
	void reload(std::istream& user_config, std::istream& default_config);
};

} // namespace config
} // namespace mce

#endif /* CONFIG_CONFIG_STORE_HPP_ */
