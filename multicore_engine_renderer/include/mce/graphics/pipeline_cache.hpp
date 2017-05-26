/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/pipeline_cache.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_CACHE_HPP_
#define GRAPHICS_PIPELINE_CACHE_HPP_

#include <mce/graphics/unique_handle.hpp>
#include <string>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class window;
class device;

class pipeline_cache {
private:
	device& device_;
	unique_handle<vk::PipelineCache> native_pipeline_cache_;
	uint8_t uuid_[VK_UUID_SIZE];
	std::string uuid_str_;
	std::string cache_filename_;
	bool file_read_only_;

	std::vector<char> read_file(const std::string& filename);
	void write_file(const std::string& filename, const std::vector<char>& content);

public:
	explicit pipeline_cache(device& dev, bool file_read_only = false);
	~pipeline_cache();

	const vk::PipelineCache& native_pipeline_cache() const {
		return native_pipeline_cache_.get();
	}

	const std::string& uuid_string() const {
		return uuid_str_;
	}

	const std::string& cache_filename() const {
		return cache_filename_;
	}

	bool file_read_only() const {
		return file_read_only_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_CACHE_HPP_ */
