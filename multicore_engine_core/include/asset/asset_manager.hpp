/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/asset_manager.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_MANAGER_HPP_
#define ASSET_ASSET_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>
#include <thread>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4005)
#endif
#include <boost/thread/future.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/asio/io_service.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace mce {
namespace asset {
class asset_loader;
class asset;

class asset_manager {
	std::vector<std::unique_ptr<asset_loader>> asset_loaders;
	std::shared_timed_mutex loaded_assets_rw_lock;
	boost::container::flat_map<std::string, std::shared_ptr<asset>> loaded_assets;
	boost::asio::io_service task_pool;
	std::vector<std::thread> workers;
	std::unique_ptr<boost::asio::io_service::work> work;

	struct future_load_task {
		std::shared_ptr<boost::promise<std::shared_ptr<const asset>>> promise;
		std::string name;
		asset_manager* manager;
		future_load_task(const std::string& name, asset_manager* manager)
				: promise(std::make_shared<boost::promise<std::shared_ptr<const asset>>>()), name{name},
				  manager{manager} {}
		void operator()();
	};
	std::shared_ptr<const asset> load_asset_sync_core(const std::string& name);
	std::shared_ptr<const asset> call_loaders_sync(const std::shared_ptr<asset>& asset) const;

public:
	asset_manager();
	~asset_manager();
	template <typename F>
	std::shared_ptr<const asset> load_asset_async(const std::string& name, F completion_handler);
	std::shared_ptr<const asset> load_asset_sync(const std::string& name);
	boost::unique_future<std::shared_ptr<const asset>> load_asset_future(const std::string& name);
	void clean();
	void pin_load_unit(const std::string& name);
	void unpin_load_unit(const std::string& name);
};

} // namespace asset
} // namespace mce

#include "asset.hpp"
#include "asset_loader.hpp"

namespace mce {
namespace asset {

template <typename F>
std::shared_ptr<const asset> asset_manager::load_asset_async(const std::string& name, F completion_handler) {
	std::shared_ptr<const asset> result;
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		auto it = loaded_assets.find(name);
		if(it != loaded_assets.end()) { result = it->second; }
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
			tmp->run_when_loaded(completion_handler);
			task_pool.post([tmp, this]() {
				if(tmp->try_obtain_load_ownership()) {
					for(auto& loader : asset_loaders) {
						if(loader->start_load_asset(tmp)) return;
					}
					tmp->raise_error_flag();
				}
			});
			return tmp;
		}
	}

	result->run_when_loaded(completion_handler);
	return result;
}

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_MANAGER_HPP_ */
