/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/graphics/pipeline_cache.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <fstream>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {

boost::filesystem::path pipeline_cache::cache_path_ = ".";

std::vector<char> pipeline_cache::read_file(const std::string& filename) {
	std::vector<char> res;
	std::ifstream stream(filename, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		size_t size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw std::runtime_error("file too big to fit in address space.");
		stream.seekg(0, std::ios::beg);
		res.resize(size);
		stream.read(res.data(), size);
		stream.close();
	}
	return res;
}

void pipeline_cache::write_file(const std::string& filename, const std::vector<char>& content) {
	std::ofstream stream(filename, std::ios::binary | std::ios::trunc);
	if(stream) {
		stream.write(content.data(), content.size());
		stream.close();
	}
}

pipeline_cache::pipeline_cache(device& dev, bool file_read_only)
		: device_(dev), file_read_only_{file_read_only} {
	std::copy(dev.physical_device_properties().pipelineCacheUUID,
			  dev.physical_device_properties().pipelineCacheUUID + VK_UUID_SIZE, uuid_);
	uuid_str_.reserve(2 * VK_UUID_SIZE);
	auto hex_digit = [](auto val) -> char { return char((val < 10) ? ('0' + val) : ('A' + (val - 10))); };
	for(int i = 0; i < VK_UUID_SIZE; ++i) {
		if(i % 4 == 0 && i != 0) uuid_str_ += '-';
		uuid_str_ += hex_digit((uuid_[i] & 0xF0u) >> 4);
		uuid_str_ += hex_digit(uuid_[i] & 0x0Fu);
	}
	boost::filesystem::create_directories(cache_path_);
	auto file = read_file(cache_filename().string());
	vk::PipelineCacheCreateInfo pipeline_cache_ci;
	if(!file.empty()) {
		pipeline_cache_ci.pInitialData = file.data();
		pipeline_cache_ci.initialDataSize = file.size();
	}
	native_pipeline_cache_ = device_.native_device().createPipelineCacheUnique(pipeline_cache_ci);
}

pipeline_cache::~pipeline_cache() {
	if(!native_pipeline_cache_) return;
	if(file_read_only_) return;
	size_t data_size = 0;
	device_.native_device().getPipelineCacheData(*native_pipeline_cache_, &data_size, nullptr);
	std::vector<char> content(data_size);
	device_.native_device().getPipelineCacheData(*native_pipeline_cache_, &data_size, content.data());
	boost::filesystem::create_directories(cache_path_);
	write_file(cache_filename().string(), content);
}

} /* namespace graphics */
} /* namespace mce */
