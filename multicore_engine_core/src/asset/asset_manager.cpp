/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/asset_manager.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_ASSET_MANAGER_CPP_
#define ASSET_ASSET_MANAGER_CPP_

#include <asset/asset_manager.hpp>
#include <asset/asset_loader.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace mce {
namespace asset {

asset_manager::asset_manager() {
	work = std::make_unique<boost::asio::io_service::work>(task_pool);
	unsigned int worker_thread_count = 2 * std::min(std::thread::hardware_concurrency(), 1u);
	for(unsigned int i = 0; i < worker_thread_count; ++i) {
		workers.emplace_back([this]() { task_pool.run(); });
	}
}
asset_manager::~asset_manager() {
	work.reset();
	for(auto& worker : workers) {
		worker.join();
	}
}

void asset_manager::start_clean() {
	task_pool.post([this]() {
		std::unique_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		// TODO Validate if usage of remove_erase_if on flat_map is allowed by flat_map, otherwise possibly
		// extend and switch to generic_flat_map
		boost::remove_erase_if(loaded_assets, [](const auto& element) { return element.second.unique(); });
	});
}
std::shared_ptr<const asset> asset_manager::call_loaders_sync(const std::shared_ptr<asset>& asset_to_load) {
	if(asset_to_load->try_obtain_load_ownership()) {
		try {
			auto local_asset_loaders = asset_loaders.get();
			for(auto& loader : *local_asset_loaders) {
				if(loader->start_load_asset(asset_to_load, *this)) {
					asset_to_load->internal_wait_for_complete();
					return asset_to_load;
				}
			}
		} catch(...) {
			asset_to_load->raise_error_flag();
			throw;
		}
		asset_to_load->raise_error_flag();
		asset_to_load->check_error_flag();
		return std::shared_ptr<const asset>();
	} else {
		asset_to_load->internal_wait_for_complete();
		asset_to_load->check_error_flag();
		return asset_to_load;
	}
}

std::shared_ptr<const asset> asset_manager::load_asset_sync_core(const std::string& name) {
	// Acquire write lock
	std::unique_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
	auto it = loaded_assets.find(name);
	if(it != loaded_assets.end()) {
		auto res = it->second;
		lock.unlock();
		return call_loaders_sync(res);
	} else {
		std::shared_ptr<asset> result = std::make_shared<asset>(name);
		loaded_assets[name] = result;
		lock.unlock();
		return call_loaders_sync(result);
	}
}

std::shared_ptr<const asset> asset_manager::load_asset_sync(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		auto it = loaded_assets.find(name);
		if(it != loaded_assets.end()) {
			auto res = it->second;
			lock.unlock();
			return call_loaders_sync(res);
		}
	}
	return load_asset_sync_core(name);
}
void asset_manager::future_load_task::operator()() {
	try {
		promise->set_value(manager->load_asset_sync_core(name));
	} catch(...) {
		try {
			promise->set_exception(std::current_exception());
		} catch(...) {
		}
	}
}
boost::unique_future<std::shared_ptr<const asset>> asset_manager::load_asset_future(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_assets_rw_lock);
		auto it = loaded_assets.find(name);
		if(it != loaded_assets.end()) {
			if(it->second->ready()) return boost::make_ready_future(std::shared_ptr<const asset>(it->second));
		}
	}
	future_load_task load_task{name, this};
	auto future = load_task.promise->get_future();
	task_pool.post(load_task);
	return future;
}
void asset_manager::start_pin_load_unit(const std::string& name) {
	auto local_asset_loaders = asset_loaders.get();
	for(auto& loader : *local_asset_loaders) {
		loader->start_pin_load_unit(name, *this);
	}
}
void asset_manager::start_pin_load_unit(const std::string& name,
										const simple_completion_handler& completion_handler) {
	auto local_asset_loaders = asset_loaders.get();
	for(auto& loader : *local_asset_loaders) {
		loader->start_pin_load_unit(name, *this, completion_handler);
	}
}
void asset_manager::start_unpin_load_unit(const std::string& name) {
	auto local_asset_loaders = asset_loaders.get();
	for(auto& loader : *local_asset_loaders) {
		loader->start_unpin_load_unit(name, *this);
	}
}

void asset_manager::add_asset_loader(std::shared_ptr<asset_loader>&& loader) {
	asset_loaders.do_transaction([&](auto& loaders) { loaders.emplace_back(std::move(loader)); });
}
void asset_manager::clear_asset_loaders() {
	asset_loaders.do_transaction([&](auto& loaders) { loaders.clear(); });
}

} // namespace asset
} // namespace mce

#endif /* ASSET_ASSET_MANAGER_CPP_ */
