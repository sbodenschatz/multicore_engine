/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset_loader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset_loader.hpp>
#include <asset/asset.hpp>

namespace mce {
namespace asset {

void asset_loader::finish_loading(const std::shared_ptr<asset>& asset, std::shared_ptr<const char> data,
								  size_t size) {
	asset->complete_loading(data, size);
}

} // namespace asset
} // namespace mce
