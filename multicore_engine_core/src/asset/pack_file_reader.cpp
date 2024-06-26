/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/pack_file_reader.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/vector.hpp>
#include <cassert>
#include <cstring>
#include <iterator>
#include <mce/asset/pack_file_reader.hpp>
#include <mce/exceptions.hpp>
#include <mce/util/compression.hpp>
#include <mutex>

namespace mce {
namespace asset {

util::lock_ptr_wrapper<pack_file_reader::pack_file_source>
pack_file_reader::get_source_stream(const std::string& prefix) {
	{
		// Take read lock
		std::shared_lock<std::shared_timed_mutex> lock(sources_rw_lock);
		// Try to find an existing and unused source stream for the given pack file (prefix)
		auto equal_range = opened_pack_file_sources.equal_range(prefix);
		for(auto it = equal_range.first; it != equal_range.second; ++it) {
			if(it->second) {
				util::lock_ptr_wrapper<pack_file_source> lock_ptr(it->second, std::try_to_lock);
				if(lock_ptr) return lock_ptr;
			}
		}
	}
	// No source stream for the pack file exists or none of them is free, allocate a new one to avoid waiting
	// for other threads IO
	auto ptr = std::make_shared<pack_file_source>(prefix);
	util::lock_ptr_wrapper<pack_file_source> lock_ptr(ptr, std::try_to_lock);
	assert(lock_ptr);
	// Take write lock
	std::unique_lock<std::shared_timed_mutex> lock(sources_rw_lock);
	opened_pack_file_sources.emplace(prefix, std::move(ptr));
	return lock_ptr;
}

std::pair<file_content_ptr, file_size> pack_file_reader::read_file(const std::string& prefix,
																   const std::string& file) {
	try {
		auto source = get_source_stream(prefix);
		auto pos = std::find_if(source->metadata.elements.begin(), source->metadata.elements.end(),
								[&](const pack_file_element_meta_data& elem) { return elem.name == file; });
		if(pos == source->metadata.elements.end()) {
			return std::make_pair(file_content_ptr(), 0ull);
		} else if(pos->compressed_size == 0) {
			std::shared_ptr<char> content =
					std::shared_ptr<char>(new char[pos->size], [](char* ptr) { delete[] ptr; });
			source->stream.seekg(pos->offset, std::ios::beg);
			source->stream.read(content.get(), pos->size);
			if(!(source->stream))
				return std::make_pair(file_content_ptr(), 0ull);
			else
				return std::make_pair(content, pos->size);
		} else {
			source->compressed_buffer.clear();
			source->compressed_buffer.resize(pos->compressed_size, '\0');
			source->stream.seekg(pos->offset, std::ios::beg);
			source->stream.read(source->compressed_buffer.data(), pos->compressed_size);
			if(!(source->stream)) {
				return std::make_pair(file_content_ptr(), 0ull);
			} else {
				source->decompressed_buffer.clear();
				util::decompress(source->compressed_buffer, source->decompressed_buffer);
				if(source->decompressed_buffer.size() != pos->size)
					throw io_exception(
							"Pack file appears corrupt: asset size and decompressed data size differ.");
				std::shared_ptr<char> content =
						std::shared_ptr<char>(new char[pos->size], [](char* ptr) { delete[] ptr; });
				std::memcpy(content.get(), source->decompressed_buffer.data(), pos->size);
				return std::make_pair(content, pos->size);
			}
		}
	} catch(...) {
		return std::make_pair(file_content_ptr(), 0ull);
	}
}

} // namespace asset
} // namespace mce
