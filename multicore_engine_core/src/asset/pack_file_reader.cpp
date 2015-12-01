/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/pack_file_reader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/pack_file_reader.hpp>
#include <fstream>
#include <util/finally.hpp>
#include <algorithm>

namespace mce {
namespace asset {

std::shared_ptr<pack_file_reader::pack_file_source>
pack_file_reader::get_source_stream(const std::string& prefix) {
	{
		// Take read lock
		std::shared_lock<std::shared_timed_mutex> lock(sources_rw_lock);
		auto equal_range = opened_pack_file_sources.equal_range(prefix);
		for(auto it = equal_range.first; it != equal_range.second; ++it) {
			if(it->second) {
				if(it->second->try_lock()) return it->second;
			}
		}
	}
	auto ptr = std::make_shared<pack_file_source>(prefix);
	ptr->try_lock();
	// Take write lock
	std::unique_lock<std::shared_timed_mutex> lock(sources_rw_lock);
	opened_pack_file_sources.emplace(prefix, ptr);
	return ptr;
}

std::tuple<file_reader::file_content_ptr, file_reader::file_size>
pack_file_reader::read_file(const std::string& prefix, const std::string& file) {
	auto source = get_source_stream(prefix);
	auto at_exit = util::finally([&]() { source->unlock(); });
	auto pos = std::find_if(source->metadata.elements.begin(), source->metadata.elements.end(),
							[&](const pack_file_element_meta_data& elem) { return elem.name == file; });
	if(pos == source->metadata.elements.end()) { return std::make_tuple(file_content_ptr(), 0ull); } else {
		std::shared_ptr<char> content =
				std::shared_ptr<char>(new char[pos->size], [](char* ptr) { delete[] ptr; });
		source->stream.seekg(pos->offset, std::ios::beg);
		source->stream.read(content.get(), pos->size);
		if(!(source->stream))
			return std::make_tuple(file_content_ptr(), 0ull);
		else
			return std::make_tuple(content, pos->size);
	}
}

} // namespace asset
} // namespace mce
