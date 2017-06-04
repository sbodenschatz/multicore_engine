/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/dummy_asset.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset.hpp>
#include <mce/asset/dummy_asset.hpp>
#include <cstring>
#include <memory>

namespace mce {
namespace asset {

asset_ptr dummy_asset::create_dummy_asset(const std::string& name, const std::string& content) {
	std::shared_ptr<char> content_mem =
			std::shared_ptr<char>(new char[content.size()], [](char* ptr) { delete[] ptr; });
	std::memcpy(content_mem.get(), content.data(), content.size());
	auto res = std::make_shared<asset>(name);
	res->complete_loading(content_mem, content.size());
	return res;
}
asset_ptr dummy_asset::create_dummy_asset(const std::string& name, const std::vector<char>& content) {
	std::shared_ptr<char> content_mem =
			std::shared_ptr<char>(new char[content.size()], [](char* ptr) { delete[] ptr; });
	std::memcpy(content_mem.get(), content.data(), content.size());
	auto res = std::make_shared<asset>(name);
	res->complete_loading(content_mem, content.size());
	return res;
}

} // namespace asset
} // namespace mce
