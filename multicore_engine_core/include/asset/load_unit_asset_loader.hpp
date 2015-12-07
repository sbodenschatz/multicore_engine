/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/load_unit_asset_loader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_LOAD_UNIT_ASSET_LOADER_HPP_
#define ASSET_LOAD_UNIT_ASSET_LOADER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <shared_mutex>
#include "asset_loader.hpp"
#include "asset_defs.hpp"
#include <containers/scratch_pad_pool.hpp>

namespace mce {
namespace asset {
class load_unit;

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
	explicit load_unit_asset_loader(const std::vector<path_prefix>& prefixes);
	explicit load_unit_asset_loader(std::vector<path_prefix>&& prefixes);
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset, asset_manager& asset_manager) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager) override;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager,
									 const simple_completion_handler& completion_handler) override;
	virtual void start_unpin_load_unit(const std::string& name, asset_manager& asset_manager) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_LOAD_UNIT_ASSET_LOADER_HPP_ */
