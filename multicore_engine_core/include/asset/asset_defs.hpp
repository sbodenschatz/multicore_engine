/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_defs.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_DEFS_HPP_
#define ASSET_ASSET_DEFS_HPP_

#include <exception>
#include <util/local_function.hpp>

namespace mce {
namespace asset {

typedef util::local_function<128, void()> simple_completion_handler;

class load_unit;
typedef std::shared_ptr<const load_unit> load_unit_ptr;
typedef util::local_function<128, void(const load_unit_ptr& load_unit)> load_unit_completion_handler;

class asset;
typedef std::shared_ptr<const asset> asset_ptr;
typedef util::local_function<128, void(const asset_ptr& asset)> asset_completion_handler;

typedef std::shared_ptr<const char> file_content_ptr;
typedef size_t file_size;

struct offset_range {
	file_size begin;
	file_size end;
};

typedef util::local_function<128, void(std::exception_ptr)> error_handler;

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_DEFS_HPP_ */
