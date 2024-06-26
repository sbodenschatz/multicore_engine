/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/file_asset_loader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/asset/asset.hpp>
#include <mce/asset/file_asset_loader.hpp>
#include <mce/asset/file_reader.hpp>
#include <mce/util/local_function.hpp>
#include <mutex>
#include <tuple>
#include <utility>

namespace mce {
namespace asset {
file_asset_loader::file_asset_loader(const std::vector<path_prefix>& prefixes) : prefixes(prefixes) {
	load_units.push_back("");
}

file_asset_loader::file_asset_loader(std::vector<path_prefix>&& prefixes) : prefixes(std::move(prefixes)) {
	load_units.push_back("");
}

bool file_asset_loader::start_load_asset(const std::shared_ptr<asset>& asset, asset_manager&, bool) {
	std::shared_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
	std::string file_path;
	file_path.reserve(128);
	for(const auto& prefix : prefixes) {
		for(const auto& load_unit : load_units) {
			if(!load_unit.empty()) {
				file_path = load_unit;
				file_path += '/';
			} else {
				file_path = "";
			}
			file_path += asset->name();
			file_content_ptr file_content;
			file_size file_size = 0u;
			std::tie(file_content, file_size) = prefix.reader->read_file(prefix.prefix, file_path);
			if(file_content) {
				lock.unlock();
				finish_loading(asset, file_content, file_size);
				return true;
			}
		}
	}
	return false;
}
void file_asset_loader::start_pin_load_unit(const std::string& name, asset_manager&) {
	std::unique_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
	load_units.push_back(name);
}
void file_asset_loader::start_pin_load_unit(const std::string& name, asset_manager& manager,
											const simple_completion_handler& completion_handler,
											const error_handler&) {
	// In this class the start_pin_load_unit method is blocking (only adds an entry to load_units list),
	// therefore delegate to non-completion-handler implementation and run completion handler after this.
	start_pin_load_unit(name, manager);
	completion_handler();
}
void file_asset_loader::start_unpin_load_unit(const std::string& name, asset_manager&) {
	std::unique_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
	load_units.erase(std::remove(load_units.begin(), load_units.end(), name), load_units.end());
}

} // namespace asset
} // namespace mce
