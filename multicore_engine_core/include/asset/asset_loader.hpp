/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_loader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_LOADER_HPP_
#define ASSET_ASSET_LOADER_HPP_

#include <string>
#include <memory>

namespace mce {
namespace asset {
class asset;
class asset_manager;

class asset_loader {
protected:
	void finish_loading(const std::shared_ptr<asset>& asset, std::shared_ptr<const char> data, size_t size);

public:
	virtual ~asset_loader() = default;
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset) = 0;
	virtual void pin_load_unit(const std::string& name, asset_manager& asset_manager) = 0;
	virtual void unpin_load_unit(const std::string& name, asset_manager& asset_manager) = 0;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_LOADER_HPP_ */
