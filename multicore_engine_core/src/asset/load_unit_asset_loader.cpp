/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/load_unit_asset_loader.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <mce/asset/asset.hpp>
#include <mce/asset/file_reader.hpp>
#include <mce/asset/load_unit.hpp>
#include <mce/asset/load_unit_asset_loader.hpp>
#include <mce/exceptions.hpp>
#include <algorithm>
#include <exception>
#include <iterator>
#include <mutex>
#include <shared_mutex>
#include <tuple>

namespace mce {
namespace asset {
load_unit_asset_loader::load_unit_asset_loader(const std::vector<path_prefix>& prefixes)
		: prefixes(prefixes) {}

load_unit_asset_loader::load_unit_asset_loader(std::vector<path_prefix>&& prefixes)
		: prefixes(std::move(prefixes)) {}

std::pair<file_content_ptr, file_size>
load_unit_asset_loader::load_file_from_prefixes(const std::string& name) const {
	for(const auto& prefix : prefixes) {
		auto file = prefix.reader->read_file(prefix.prefix, name);
		if(file.first) {
			return file;
		}
	}
	return std::make_pair(file_content_ptr(), file_size(0));
}
void load_unit_asset_loader::start_payload_loading(const std::shared_ptr<load_unit>& load_unit,
												   asset_manager& asset_manager) const {
	launch_async_task(asset_manager, [load_unit, this]() {
		if(load_unit->try_obtain_data_load_ownership()) {
			try {
				file_content_ptr content;
				file_size size;
				std::tie(content, size) = load_file_from_prefixes(load_unit->name() + ".lup");
				if(content) {
					load_unit->complete_loading(content, size);
				} else {
					load_unit->raise_error_flag(std::make_exception_ptr(path_not_found_exception(
							"Couldn't load payload data for load unit '" + load_unit->name() + "'.")));
				}
			} catch(...) {
				load_unit->raise_error_flag(std::current_exception());
			}
		}
	});
}

void load_unit_asset_loader::prepare_load_unit_meta_data(const std::shared_ptr<load_unit>& load_unit,
														 asset_manager& asset_manager) const {
	if(!load_unit->meta_data_ready()) {
		if(load_unit->try_obtain_meta_load_ownership()) {
			file_content_ptr content;
			try {
				file_size size;
				std::tie(content, size) = load_file_from_prefixes(load_unit->name() + ".lum");
				if(content) {
					load_unit->load_meta_data(content, size);
					start_payload_loading(load_unit, asset_manager);
				} else {
					load_unit->raise_error_flag(std::make_exception_ptr(path_not_found_exception(
							"Couldn't load meta data for load unit '" + load_unit->name() + "'.")));
				}
			} catch(...) {
				load_unit->raise_error_flag(std::current_exception());
				throw;
			}
			load_unit->check_error_flag();
		} else {
			load_unit->internal_wait_for_meta_complete();
		}
	}
}

bool load_unit_asset_loader::start_load_asset(const std::shared_ptr<asset>& asset,
											  asset_manager& asset_manager, bool sync_hint) {
	auto local_load_units = load_unit_scratch.get();
	{
		std::shared_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
		*local_load_units = load_units;
	}
	for(const auto& load_unit : *local_load_units) {
		try {
			prepare_load_unit_meta_data(load_unit, asset_manager);
			auto resolution_cookie = load_unit->resolve_asset(asset->name());
			if(resolution_cookie) {
				load_unit->run_when_loaded(
						[asset, resolution_cookie](const load_unit_ptr& load_unit) {
							file_content_ptr content;
							file_size size;
							std::tie(content, size) = load_unit->get_asset_content(resolution_cookie);
							if(content) {
								finish_loading(asset, content, size);
							} else {
								raise_error_flag(asset,
												 std::make_exception_ptr(path_not_found_exception(
														 "Couldn't load asset '" + asset->name() +
														 "' from load unit '" + load_unit->name() + "'.")));
							}
						},
						[asset](std::exception_ptr e) { raise_error_flag(asset, e); });
				if(sync_hint) {
					if(!load_unit->ready()) {
						if(load_unit->try_obtain_data_load_ownership()) {
							try {
								file_content_ptr content;
								file_size size;
								std::tie(content, size) = load_file_from_prefixes(load_unit->name() + ".lup");
								if(content) {
									load_unit->complete_loading(content, size);
								} else {
									load_unit->raise_error_flag(
											std::make_exception_ptr(path_not_found_exception(
													"Couldn't load payload data for load unit '" +
													load_unit->name() + "'.")));
								}
							} catch(...) {
								load_unit->raise_error_flag(std::current_exception());
							}
						}
					}
				}
				return true;
			}
		} catch(...) {
		}
	}
	return false;
}

std::shared_ptr<load_unit> load_unit_asset_loader::start_pin_load_unit_helper(const std::string& name,
																			  asset_manager& manager) {
	std::shared_ptr<load_unit> load_unit_ptr;
	{
		std::unique_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
		auto it = std::find_if(load_units.begin(), load_units.end(),
							   [&name](const auto& load_unit) { return name == load_unit->name(); });
		if(it != load_units.end()) {
			return *it;
		} else {
			load_unit_ptr = std::make_shared<load_unit>(name);
			load_units.emplace_back(load_unit_ptr);
		}
	}
	launch_async_task(manager, [this, load_unit_ptr, &manager]() {
		if(load_unit_ptr->try_obtain_meta_load_ownership()) {
			try {
				file_content_ptr content;
				file_size size;
				std::tie(content, size) = load_file_from_prefixes(load_unit_ptr->name() + ".lum");
				if(content) {
					load_unit_ptr->load_meta_data(content, size);
					start_payload_loading(load_unit_ptr, manager);
				} else {
					load_unit_ptr->raise_error_flag(std::make_exception_ptr(path_not_found_exception(
							"Couldn't load meta data for load unit '" + load_unit_ptr->name() + "'.")));
				}
			} catch(...) {
				load_unit_ptr->raise_error_flag(std::current_exception());
			}
		}
	});
	return load_unit_ptr;
}

void load_unit_asset_loader::start_pin_load_unit(const std::string& name, asset_manager& manager) {
	start_pin_load_unit_helper(name, manager);
}
void load_unit_asset_loader::start_pin_load_unit(const std::string& name, asset_manager& manager,
												 const simple_completion_handler& completion_handler,
												 const error_handler& error_handler) {
	auto load_unit = start_pin_load_unit_helper(name, manager);
	load_unit->run_when_loaded_simple(completion_handler, error_handler);
}
void load_unit_asset_loader::start_unpin_load_unit(const std::string& name, asset_manager&) {
	std::unique_lock<std::shared_timed_mutex> lock(load_units_rw_lock);
	load_units.erase(std::remove_if(load_units.begin(), load_units.end(), [&name](const auto& load_unit) {
		return load_unit->name() == name;
	}), load_units.end());
}
} // namespace asset
} // namespace mce
