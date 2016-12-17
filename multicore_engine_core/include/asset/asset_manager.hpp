/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_manager.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_MANAGER_HPP_
#define ASSET_ASSET_MANAGER_HPP_

#include "asset_defs.hpp"
#include <exception>
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>
#include <util/copy_on_write.hpp>
#include <util/unused.hpp>
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
	asset_manager();
	~asset_manager();
	asset_manager(const asset_manager&) = delete;
	asset_manager& operator=(const asset_manager&) = delete;
	template <typename F>
	std::shared_ptr<const asset> load_asset_async(const std::string& name, F&& completion_handler) {
		return load_asset_async(name, std::forward<F>(completion_handler), [](std::exception_ptr) {});
	}
	template <typename F, typename E>
	std::shared_ptr<const asset> load_asset_async(const std::string& name, F completion_handler,
												  E error_handler);
	std::shared_ptr<const asset> load_asset_sync(const std::string& name);
	boost::unique_future<std::shared_ptr<const asset>> load_asset_future(const std::string& name);
	void start_clean();
	void start_pin_load_unit(const std::string& name);
	void start_pin_load_unit(const std::string& name, const simple_completion_handler& completion_handler,
							 const error_handler& error_handler);
	void start_unpin_load_unit(const std::string& name);

	void add_asset_loader(std::shared_ptr<asset_loader>&& loader);
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
							std::runtime_error("Couldn't find asset '" + tmp->name() +
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
