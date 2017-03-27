/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_loader.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_LOADER_HPP_
#define ASSET_ASSET_LOADER_HPP_

#include "asset_defs.hpp"
#include <exception>
#include <memory>
#include <string>

namespace mce {
namespace asset {
class asset;
class asset_manager;
class file_reader;

struct path_prefix {
	std::shared_ptr<file_reader> reader;
	std::string prefix;
};

class asset_loader {
protected:
	template <typename F>
	static void launch_async_task(asset_manager& asset_manager, F&& f);
	static void finish_loading(const std::shared_ptr<mce::asset::asset>& asset, const file_content_ptr& data,
							   file_size size);
	static void raise_error_flag(const std::shared_ptr<mce::asset::asset>& asset, std::exception_ptr e);

public:
	virtual ~asset_loader() = default;
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset, asset_manager& asset_manager,
								  bool sync_hint) = 0;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager) = 0;
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager,
									 const simple_completion_handler& completion_handler,
									 const error_handler& error_handler) = 0;
	virtual void start_unpin_load_unit(const std::string& name, asset_manager& asset_manager) = 0;
};

} // namespace asset
} // namespace mce

#include "asset_manager.hpp"

namespace mce {
namespace asset {
template <typename F>
void asset_loader::launch_async_task(asset_manager& asset_manager, F&& f) {
	asset_manager.task_pool.post(std::forward<F>(f));
}

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_LOADER_HPP_ */
