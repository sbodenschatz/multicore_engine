/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_loader.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_LOADER_HPP_
#define ASSET_ASSET_LOADER_HPP_

/**
 * \file
 * Defines the interface for asset loaders.
 */

#include <asset/asset_defs.hpp>
#include <boost/asio/io_service.hpp>
#include <exception>
#include <memory>
#include <string>

namespace mce {
namespace asset {
class asset;
class asset_manager;
class file_reader;

/// \brief Represents a combination of a file path prefix and a file_reader that are used together by most
/// asset_loaders to specify where to load assets from.
struct path_prefix {
	std::shared_ptr<file_reader> reader; ///< A shared_ptr to the file_reader implementation.
	std::string prefix; ///< The prefix of the path supplied to the file_reader implementation.
};

/// Provides the abstract base class for asset_loader implementations.
/**
 * Implementations of this class are used by the asset_manager to define loading mechanisms  for assets.
 */
class asset_loader {
protected:
	/// Provides implementations with a way to launch arbitrary task function objects on the asset_manager.
	template <typename F>
	static void launch_async_task(asset_manager& asset_manager, F&& f);
	/// \brief Provides implementations with a way to complete the loading process of the given asset with the
	/// given content.
	static void finish_loading(const std::shared_ptr<mce::asset::asset>& asset, const file_content_ptr& data,
							   file_size size);
	/// Provides implementations with a way to set the error flag for the given asset.
	static void raise_error_flag(const std::shared_ptr<mce::asset::asset>& asset, std::exception_ptr e);

public:
	/// Allow polymorphic destruction.
	virtual ~asset_loader() = default;
	/// Hook function called when the loading process for an asset is started.
	virtual bool start_load_asset(const std::shared_ptr<asset>& asset, asset_manager& asset_manager,
								  bool sync_hint) = 0;
	/// Hook function called when a load unit is made active.
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager) = 0;
	/// Hook function called when a load unit is made active with given error and completion handlers.
	virtual void start_pin_load_unit(const std::string& name, asset_manager& asset_manager,
									 const simple_completion_handler& completion_handler,
									 const error_handler& error_handler) = 0;
	/// Hook function called when a load unit is deactivated.
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
