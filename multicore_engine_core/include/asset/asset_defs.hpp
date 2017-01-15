/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_defs.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_DEFS_HPP_
#define ASSET_ASSET_DEFS_HPP_

#include <exception>
#include <util/local_function.hpp>

namespace mce {
namespace asset {

/// Definition for simple completion handler callbacks.
typedef util::local_function<128, void()> simple_completion_handler;

class load_unit;
/// Definition for the owning pointers to load units.
typedef std::shared_ptr<const load_unit> load_unit_ptr;
/// Definition for completion handler callbacks used when load units have finished loading.
typedef util::local_function<128, void(const load_unit_ptr& load_unit)> load_unit_completion_handler;

class asset;
/// Definition for the owning pointers to assets.
typedef std::shared_ptr<const asset> asset_ptr;
/// Definition for completion handler callbacks used when assets have finished loading.
typedef util::local_function<128, void(const asset_ptr& asset)> asset_completion_handler;

/// Definition for the owning pointers to file contents.
typedef std::shared_ptr<const char> file_content_ptr;
/// Type for size values of files.
typedef size_t file_size;

/// Definition for callbacks used when errors are encountered while loading assets or load units.
typedef util::local_function<128, void(std::exception_ptr)> error_handler;

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_DEFS_HPP_ */
