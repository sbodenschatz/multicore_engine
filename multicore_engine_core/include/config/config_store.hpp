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

/**
 * \file
 * Defines the configuration storage class interface.
 */

namespace mce {
namespace config {

/// Implements the storage for global configuration variables in the engine.
/**
 * The configuration can be loaded from a default config file and a user specific config file and can be
 * automatically saved on destruction using a callback.
 * The configuration variables are looked up by name, providing a variable type and optionally a default
 * variable, are shared between independent lookups and can be used for read and write access.
 * The configuration system provides thread-safe access to variables as well as the config_store.
 */
class config_store {
public:
	/// \brief Access token class to allow save callbacks to store the user configuration data in
	/// callback-provided streams.
	class config_storer {
		config_store& store_;
		explicit config_storer(config_store& owner) : store_{owner} {}

		friend class config_store;

	public:
		/// Stores the user configuration data in the provided ostream.
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
	/// \brief Constructs a config_store from the given streams for the user and default config files and
	/// using the given callback for saving.
	/**
	 * The save callback must be callable with the signature <code>void(config_storer&)</code>.
	 */
	template <typename F>
	config_store(std::istream& user_config, std::istream& default_config, F&& save_callback)
			: save_callback_{std::forward<F>(save_callback)} {
		load_internal(user_config, default_config);
	}
	/// \brief Constructs a config_store from the given stream for the user config file and using the given
	/// callback for saving.
	/**
	 * The save callback must be callable with the signature <code>void(config_storer&)</code>.
	 */
	template <typename F>
	config_store(std::istream& user_config, F&& save_callback)
			: save_callback_{std::forward<F>(save_callback)} {
		read_config_file_data(user_config, true);
		parse_data();
	}
	/// \brief Constructs an empty config_store using the given callback for saving.
	/**
	 * The save callback must be callable with the signature <code>void(config_storer&)</code>.
	 */
	template <typename F>
	explicit config_store(F&& save_callback) : save_callback_{std::forward<F>(save_callback)} {}

	/// Calls the save callback and then destroys the config_store.
	~config_store() noexcept;
	/// Calls the save callback.
	void save();
	/// \brief Discards the current configuration and replaces it with the configuration resulting from the
	/// given streams for default and user config files.
	void reload(std::istream& user_config, std::istream& default_config);
	/// Looks up the variable of type T with the given name.
	/**
	 * If the variable was already looked up the variable is reused and shared.
	 * Otherwise, if the config file data contains a valid value for the variable it is created with this
	 * value. If the value in the file data is invalid or the variable is not given in the config file data,
	 * the default_value parameter is used.
	 * If the variable was already looked up with a different type, an exception of type
	 * config_variable_conflict is thrown.
	 */
	template <typename T>
	std::shared_ptr<variable<T>> resolve(const std::string& name, const T& default_value = T()) {
		std::lock_guard<std::mutex> lock(config_mutex);
		auto it = variables_.find(name);
		if(it != variables_.end()) {
			auto var = it->second->as_type<T>();
			if(!var) {
				throw config_variable_conflict_exception("Redefinition of variable '" + name +
														 "' with different type.");
			}
			return var;
		} else {
			std::shared_ptr<variable<T>> var =
					std::make_shared<variable<T>>(name, typename variable<T>::construction_key_token());
			auto it_cfg = config_file_data_.find(name);
			if(it_cfg == config_file_data_.end()) {
				var->value_from_store(default_value);
			} else {
				if(!var->parse_value_from_string_from_store(it_cfg->second)) {
					var->value_from_store(default_value);
				}
			}
			variables_.emplace(name, var);
			return var;
		}
	}
};

} // namespace config
} // namespace mce

#endif /* CONFIG_CONFIG_STORE_HPP_ */
