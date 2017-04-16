/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_manager.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_MANAGER_HPP_
#define ASSET_ASSET_MANAGER_HPP_

/**
 * \file
 * Defines the asset_manager class, the central class of the asset system.
 */

#include <algorithm>
#include <asset/asset_defs.hpp>
#include <boost/container/vector.hpp>
#include <exception>
#include <exceptions.hpp>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <util/copy_on_write.hpp>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4005)
#endif
#include <boost/asio/io_service.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/thread/future.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace mce {
namespace asset {
class asset_loader;
class asset;

/// Manages the loading and retention of asset data in the engine.
class asset_manager {
	util::copy_on_write<std::vector<std::shared_ptr<asset_loader>>> asset_loaders;
	std::shared_timed_mutex loaded_assets_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<asset>> loaded_assets;
	boost::asio::io_service task_pool;
	std::vector<std::thread> workers;
	std::unique_ptr<boost::asio::io_service::work> work;

	struct future_load_task {
		std::shared_ptr<boost::promise<std::shared_ptr<const asset>>> promise;
		std::string name;
		asset_manager* manager;
		// cppcheck-suppress passedByValue
		future_load_task(std::string name, asset_manager* manager)
				: promise(std::make_shared<boost::promise<std::shared_ptr<const asset>>>()),
				  name{std::move(name)}, manager{manager} {}
		void operator()();
	};
	std::shared_ptr<const asset> load_asset_sync_core(const std::string& name);
	std::shared_ptr<const asset> call_loaders_sync(const std::shared_ptr<asset>& asset_to_load);

public:
	friend class asset_loader;
	/// Initializes the asset_manager.
	/**
	 * Spawns worker threads for asynchronous tasks like asset loading and the corresponding completion
	 * handlers.
	 */
	asset_manager();
	/// Waits for all pending asynchronous tasks to complete and destroys the asset manager.
	~asset_manager();
	/// Forbids copying an asset_manager.
	asset_manager(const asset_manager&) = delete;
	/// Forbids copying an asset_manager.
	asset_manager& operator=(const asset_manager&) = delete;
	/// Asynchronously load the given asset and run the given completion handler when it is loaded.
	template <typename F>
	std::shared_ptr<const asset> load_asset_async(const std::string& name, F&& completion_handler) {
		return load_asset_async(name, std::forward<F>(completion_handler), [](std::exception_ptr) {});
	}
	/// \brief Asynchronously load the given asset and run the given completion handler when it is loaded and
	/// use the given error handler when loading fails.
	template <typename F, typename E>
	std::shared_ptr<const asset> load_asset_async(const std::string& name, F completion_handler,
												  E error_handler);
	/// Load the given asset and block the calling thread until the asset is loaded.
	std::shared_ptr<const asset> load_asset_sync(const std::string& name);
	/// Asynchronously load the given asset, signal completion using the returned future.
	boost::unique_future<std::shared_ptr<const asset>> load_asset_future(const std::string& name);
	/// Starts a cleanup task, that unloads unused assets.
	void start_clean();
	/// Start making the given load_unit available.
	void start_pin_load_unit(const std::string& name);
	/// \brief Start making the given load_unit available and call the given completion handler when done or
	/// the given error handler if failed.
	void start_pin_load_unit(const std::string& name, const simple_completion_handler& completion_handler,
							 const error_handler& error_handler);
	/// Starts a task to unload the given load_unit.
	void start_unpin_load_unit(const std::string& name);

	/// Adds the given asset_loader implementation to the search list.
	void add_asset_loader(std::shared_ptr<asset_loader>&& loader);
	/// Clears the asset_loader search list.
	void clear_asset_loaders();
};

} // namespace asset
} // namespace mce

#include "asset.hpp"
#include "asset_loader.hpp"

namespace mce {
namespace asset {

template <typename F, typename E>
std::shared_ptr<const asset> asset_manager::load_asset_async(const std::string& name, F completion_handler,
															 E error_handler) {
	std::shared_ptr<asset> result;
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		auto it = loaded_assets.find(name);
		if(it != loaded_assets.end()) {
			result = it->second;
		}
	}
	if(!result) {
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		// Double check if the asset is still not in the map.
		auto it = loaded_assets.find(name);
		if(it != loaded_assets.end()) {
			result = it->second;
		} else {
			auto tmp = std::make_shared<asset>(name);
			loaded_assets[name] = tmp;
			tmp->run_when_loaded(std::move(completion_handler), std::move(error_handler));
			task_pool.post([tmp, this]() {
				if(tmp->try_obtain_load_ownership()) {
					try {
						auto local_asset_loaders = asset_loaders.get();
						for(auto& loader : *local_asset_loaders) {
							if(loader->start_load_asset(tmp, *this, false)) return;
						}
					} catch(...) {
						tmp->raise_error_flag(std::current_exception());
						throw;
					}
					tmp->raise_error_flag(std::make_exception_ptr(
							path_not_found_exception("Couldn't find asset '" + tmp->name() +
													 "' through any of the registered loaders.")));
				}
			});
			return tmp;
		}
	}

	if(result) result->run_when_loaded(std::move(completion_handler), std::move(error_handler));
	return result;
}

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_MANAGER_HPP_ */
