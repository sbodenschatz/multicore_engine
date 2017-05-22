/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/dummy_asset.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef ASSET_DUMMY_ASSET_HPP_
#define ASSET_DUMMY_ASSET_HPP_

/**
 * \file
 * Provides a way for test code to create assets without depending on the asset system.
 */

#include <mce/asset/asset.hpp>
#include <mce/asset/asset_defs.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset {

/// Tool class to create assets from strings or char vectors (usually for testing purposes).
class dummy_asset {
public:
	/// Creates an asset with the given content.
	static asset_ptr create_dummy_asset(const std::string& name, const std::string& content);
	/// Creates an asset with the given content.
	static asset_ptr create_dummy_asset(const std::string& name, const std::vector<char>& content);
};

} // namespace asset
} // namespace mce

#endif /* ASSET_DUMMY_ASSET_HPP_ */
