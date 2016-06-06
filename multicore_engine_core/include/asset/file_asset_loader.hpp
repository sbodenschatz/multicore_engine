/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/file_asset_loader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_FILE_ASSET_LOADER_HPP_
#define ASSET_FILE_ASSET_LOADER_HPP_

#include "asset_loader.hpp"
#include <shared_mutex>
#include <string>
#include <vector>

namespace mce {
namespace asset {

class file_asset_loader : public asset_loader {
	std::shared_timed_mutex load_units_rw_lock;
	std::vector<std::string> load_units;
	const std::vector<path_prefix> prefixes;

public:
	explicit file_asset_loader(const std::vector<path_prefix>& prefixes);
	explicit file_asset_loader(std::vector<path_prefix>&& prefixes);
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset, asset_manager& asset_manager,
								  bool sync_hint) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager,
									 const simple_completion_handler& completion_handler) override;
	virtual void start_unpin_load_unit(const std::string& name, asset_manager& asset_manager) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_FILE_ASSET_LOADER_HPP_ */
