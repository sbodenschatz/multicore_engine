/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/graphics/pipeline_cache.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_PIPELINE_CACHE_HPP_
#define GRAPHICS_PIPELINE_CACHE_HPP_

/**
 * \file
 * Defines the pipeline_cache class for handling pipeline cache persistence.
 */

#include <boost/filesystem.hpp>
#include <string>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {
class window;
class device;

/// Handles the lifetime and automatic persistence of a pipeline cache.
class pipeline_cache {
private:
	device& device_;
	vk::UniquePipelineCache native_pipeline_cache_;
	uint8_t uuid_[VK_UUID_SIZE];
	std::string uuid_str_;
	bool file_read_only_;
	static boost::filesystem::path cache_path_;

	std::vector<char> read_file(const std::string& filename);
	void write_file(const std::string& filename, const std::vector<char>& content);

public:
	/// \brief Attempts to load the pipeline cache from persistence storage or provides an empty cache if no
	/// persisted cache is available.
	/**
	 * By specifying the file_read_only parameter to true persisting the cache on deconstruction can be
	 * disabled. This is useful for loading multiple caches from the same persisted cache and using them in
	 * parallel. Afterwards the read only copies can then be merged into the persisting one.
	 *
	 * Persisting is done using a file the name of which derives from the UUID of the vulkan implementation
	 * used to prevent attempting to load incompatible caches.
	 */
	explicit pipeline_cache(device& dev, bool file_read_only = false);
	/// Persists the pipeline cache unless it is read only and releases resources.
	~pipeline_cache();

	/// Provides access to the underlying pipeline cache.
	const vk::PipelineCache& native_pipeline_cache() const {
		return *native_pipeline_cache_;
	}

	/// Provides access to the underlying pipeline cache.
	vk::PipelineCache native_pipeline_cache() {
		return native_pipeline_cache_.get();
	}

	/// Provides the UUID string for the vulkan implementation (device and driver (version) specific).
	const std::string& uuid_string() const {
		return uuid_str_;
	}

	/// Provides the full filename of the cache persistence file for the implementation.
	boost::filesystem::path cache_filename() const {
		return cache_path_ / ("pipeline_cache_" + uuid_str_ + ".vkpc");
	}

	/// Indicates whether this cache is in read only mode for the persistence file.
	bool file_read_only() const {
		return file_read_only_;
	}

	/// Returns the path under which the pipeline cache file is stored.
	static const boost::filesystem::path& cache_path() {
		return cache_path_;
	}

	/// Sets the path under which the pipeline cache file is stored.
	static void cache_path(const boost::filesystem::path& cache_path) {
		cache_path_ = cache_path;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_PIPELINE_CACHE_HPP_ */
