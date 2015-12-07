/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset_loader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset_loader.hpp>
#include <asset/asset.hpp>

namespace mce {
namespace asset {

void asset_loader::finish_loading(const std::shared_ptr<asset>& asset, const file_content_ptr& data,
								  file_size size) {
	asset->complete_loading(data, size);
}
void asset_loader::raise_error_flag(const std::shared_ptr<asset>& asset) {
	asset->raise_error_flag();
}

} // namespace asset
} // namespace mce
