/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/graphics/pipeline_cache.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <fstream>
#include <mce/graphics/device.hpp>
#include <mce/graphics/pipeline_cache.hpp>
#include <mce/graphics/window.hpp>

namespace mce {
namespace graphics {

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

pipeline_cache::pipeline_cache(device& dev) : device_(dev) {
	std::copy(dev.physical_device_properties().pipelineCacheUUID,
			  dev.physical_device_properties().pipelineCacheUUID + VK_UUID_SIZE, uuid_);
	uuid_str_.reserve(2 * VK_UUID_SIZE);
	auto hex_digit = [](auto val) -> char { return char((val < 10) ? ('0' + val) : ('A' + (val - 10))); };
	for(int i = 0; i < VK_UUID_SIZE; ++i) {
		if(i % 4 == 0 && i != 0) uuid_str_ += '-';
		uuid_str_ += hex_digit((uuid_[i] & 0xF0u) >> 4);
		uuid_str_ += hex_digit(uuid_[i] & 0x0Fu);
	}
	cache_filename_ = "pipeline_cache_" + uuid_str_ + ".vkpc";
	auto file = read_file(cache_filename_);
	vk::PipelineCacheCreateInfo pipeline_cache_ci;
	if(!file.empty()) {
		pipeline_cache_ci.pInitialData = file.data();
		pipeline_cache_ci.initialDataSize = file.size();
	}
	native_pipeline_cache_ = unique_handle<vk::PipelineCache>(
			device_.native_device().createPipelineCache(pipeline_cache_ci),
			[this](vk::PipelineCache& pc, const vk::Optional<const vk::AllocationCallbacks>& alloc) {
				device_.native_device().destroyPipelineCache(pc, alloc);
			});
}

pipeline_cache::~pipeline_cache() {
	if(!native_pipeline_cache_) return;
	size_t data_size = 0;
	device_.native_device().getPipelineCacheData(*native_pipeline_cache_, &data_size, nullptr);
	std::vector<char> content(data_size);
	device_.native_device().getPipelineCacheData(*native_pipeline_cache_, &data_size, content.data());
	write_file(cache_filename_, content);
}

} /* namespace graphics */
} /* namespace mce */
