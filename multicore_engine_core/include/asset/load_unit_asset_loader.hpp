/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/load_unit_asset_loader.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_LOAD_UNIT_ASSET_LOADER_HPP_
#define ASSET_LOAD_UNIT_ASSET_LOADER_HPP_

#include <asset/asset_defs.hpp>
#include <asset/asset_loader.hpp>
#include <containers/scratch_pad_pool.hpp>
#include <memory>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>

namespace mce {
namespace asset {
class load_unit;

/// Implements loading of assets through load units that are read into memory as a whole.
class load_unit_asset_loader : public asset_loader {
	std::shared_timed_mutex load_units_rw_lock;
	std::vector<std::shared_ptr<load_unit>> load_units;
	const std::vector<path_prefix> prefixes;
	containers::scratch_pad_pool<std::vector<std::shared_ptr<load_unit>>> load_unit_scratch;

	std::pair<file_content_ptr, file_size> load_file_from_prefixes(const std::string& name) const;
	void start_payload_loading(const std::shared_ptr<load_unit>& load_unit,
							   asset_manager& asset_manager) const;
	void prepare_load_unit_meta_data(const std::shared_ptr<load_unit>& load_unit,
									 asset_manager& asset_manager) const;
	std::shared_ptr<load_unit> start_pin_load_unit_helper(const std::string& name,
														  asset_manager& asset_manager);

public:
	/// Creates a file_asset_loader using the given path prefixes and with an empty load unit prefix.
	explicit load_unit_asset_loader(const std::vector<path_prefix>& prefixes);
	/// Creates a file_asset_loader using the given path prefixes and with an empty load unit prefix.
	explicit load_unit_asset_loader(std::vector<path_prefix>&& prefixes);
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset, asset_manager& asset_manager,
								  bool sync_hint) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager,
									 const simple_completion_handler& completion_handler,
									 const error_handler& error_handler) override;
	virtual void start_unpin_load_unit(const std::string& name, asset_manager& asset_manager) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_LOAD_UNIT_ASSET_LOADER_HPP_ */
