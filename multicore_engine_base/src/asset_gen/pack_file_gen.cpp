/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset_gen/pack_file_gen.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <mce/asset_gen/pack_file_gen.hpp>
#include <mce/bstream/iostream_bstream.hpp>
#include <mce/bstream/vector_iobstream.hpp>
#include <cstdint>
#include <mce/exceptions.hpp>
#include <fstream>
#include <mce/util/compression.hpp>
#include <mce/util/path_util.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <tuple>
#include <utility>

namespace mce {
namespace asset_gen {
uint64_t pack_file_gen::read_file_size(const std::string& path) {
	std::string sanitized_path = path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		uint64_t size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw buffer_size_exception("Asset too big to fit in address space.");
		return size;
	} else
		throw path_not_found_exception("Couldn't open asset file '" + path + "'.");
}
std::pair<uint64_t, uint64_t> pack_file_gen::read_file_size_compressed(const std::string& path, int level) {
	std::string sanitized_path = path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		uint64_t size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw buffer_size_exception("Asset too big to fit in address space.");
		stream.seekg(0, std::ios::beg);
		input_buffer.clear();
		input_buffer.resize(size, '\0');
		stream.read(input_buffer.data(), size);
		util::compress(input_buffer, level, compressed_buffer);
		return std::make_pair(size, compressed_buffer.size());
	} else
		throw path_not_found_exception("Couldn't open asset file '" + path + "'.");
}
void pack_file_gen::update_content_offset(uint64_t new_content_offset) {
	for(auto& entry : entries) {
		entry.meta_data.offset -= content_offset;
		entry.meta_data.offset += new_content_offset;
	}
	next_pos -= content_offset;
	next_pos += new_content_offset;
	content_offset = new_content_offset;
}
void pack_file_gen::compile_meta_data() {
	meta_data.elements.clear();
	std::transform(entries.begin(), entries.end(), std::back_inserter(meta_data.elements),
				   [](const auto& entry) { return entry.meta_data; });
}
uint64_t pack_file_gen::calculate_meta_data_size() const {
	bstream::vector_iobstream temp_stream;
	temp_stream << meta_data;
	return temp_stream.tell_write();
}
void pack_file_gen::copy_file_content(std::fstream& into, const pack_file_entry& entry) {
	std::string sanitized_path = entry.path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		auto pos = into.tellp();
		if(pos < 0) throw io_exception("Error getting write position for file '" + entry.path + "'.");
		if(uint64_t(pos) != entry.meta_data.offset)
			throw io_exception("Offset mismatch for file '" + entry.path + "'.");
		constexpr size_t buffer_size = 1024;
		char buffer[buffer_size];
		uint64_t copied_bytes = 0;
		do {
			stream.read(buffer, buffer_size);
			into.write(buffer, stream.gcount());
			copied_bytes += stream.gcount();
		} while(stream.gcount());
		if(copied_bytes != entry.meta_data.size)
			throw io_exception("Size mismatch for file '" + entry.path + "'.");
	} else
		throw path_not_found_exception("Couldn't open asset file '" + entry.path + "'.");
}
void pack_file_gen::copy_file_content_compressed(std::fstream& into, const pack_file_entry& entry) {
	std::string sanitized_path = entry.path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		auto pos = into.tellp();
		if(pos < 0) throw io_exception("Error getting write position for file '" + entry.path + "'.");
		if(uint64_t(pos) != entry.meta_data.offset)
			throw io_exception("Offset mismatch for file '" + entry.path + "'.");
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		uint64_t size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw buffer_size_exception("Asset too big to fit in address space.");
		stream.seekg(0, std::ios::beg);
		input_buffer.clear();
		input_buffer.resize(size, '\0');
		stream.read(input_buffer.data(), size);
		util::compress(input_buffer, entry.compression_level, compressed_buffer);
		into.write(compressed_buffer.data(), compressed_buffer.size());
		if(std::max(uint64_t(stream.gcount()), uint64_t(0)) != entry.meta_data.size)
			throw io_exception("Size mismatch for file '" + entry.path + "'.");
		if(compressed_buffer.size() != entry.meta_data.compressed_size)
			throw io_exception("Compressed size mismatch for file '" + entry.path + "'.");
	} else
		throw path_not_found_exception("Couldn't open asset file '" + entry.path + "'.");
}
void pack_file_gen::write_pack_file(const std::string& output_file) {
	std::fstream file_stream(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!file_stream) throw path_not_found_exception("Can't open '" + output_file + "' for writing.");
	bstream::iostream_bstream stream(file_stream);
	stream << meta_data;
	auto test = file_stream.tellp();
	static_cast<void>(test);
	for(const auto& entry : entries) {
		if(entry.compression_level > -2) {
			copy_file_content_compressed(file_stream, entry);
		} else {
			copy_file_content(file_stream, entry);
		}
	}
}
void pack_file_gen::add_file(const std::string& path, const std::string& name) {
	auto size = read_file_size(path);
	entries.emplace_back(path, name, next_pos, size);
	next_pos += size;
}
void pack_file_gen::add_file_compressed(const std::string& path, const std::string& name, int level) {
	if(level > 9) level = 9;
	uint64_t size;
	uint64_t compressed_size;
	std::tie(size, compressed_size) = read_file_size_compressed(path, level);
	entries.emplace_back(path, name, next_pos, size, compressed_size, level);
	next_pos += compressed_size;
}
void pack_file_gen::compile_pack_file(const std::string& output_file) {
	compile_meta_data();
	auto start_offset = calculate_meta_data_size();
	update_content_offset(start_offset);
	compile_meta_data();
	auto start_offset2 = calculate_meta_data_size();
	if(start_offset != start_offset2)
		throw logic_exception("Meta data size mismatch after applying start_offset.");
	write_pack_file(output_file);
}

} // namespace asset_gen
} // namespace mce
