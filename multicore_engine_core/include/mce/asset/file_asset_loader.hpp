/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/file_asset_loader.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_FILE_ASSET_LOADER_HPP_
#define ASSET_FILE_ASSET_LOADER_HPP_

/**
 * \file
 * Defines an asset_loader that works a per file basis.
 */

#include <mce/asset/asset_defs.hpp>
#include <mce/asset/asset_loader.hpp>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

namespace mce {
namespace asset {

/// Loads assets directly from the reader and models load units as additional path prefixes.
class file_asset_loader : public asset_loader {
	std::shared_timed_mutex load_units_rw_lock;
	std::vector<std::string> load_units;
	const std::vector<path_prefix> prefixes;

public:
	/// Creates a file_asset_loader using the given path prefixes and with an empty load unit prefix.
	explicit file_asset_loader(const std::vector<path_prefix>& prefixes);
	/// Creates a file_asset_loader using the given path prefixes and with an empty load unit prefix.
	explicit file_asset_loader(std::vector<path_prefix>&& prefixes);
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

#endif /* ASSET_FILE_ASSET_LOADER_HPP_ */
