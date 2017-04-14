/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset_loader.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <asset/asset.hpp>
#include <asset/asset_loader.hpp>

namespace mce {
namespace asset {

void asset_loader::finish_loading(const std::shared_ptr<asset>& asset, const file_content_ptr& data,
								  file_size size) {
	asset->complete_loading(data, size);
}
void asset_loader::raise_error_flag(const std::shared_ptr<asset>& asset, std::exception_ptr e) {
	asset->raise_error_flag(e);
}

} // namespace asset
} // namespace mce
