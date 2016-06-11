/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset_gen/load_unit_gen.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <asset/asset_defs.hpp>
#include <asset_gen/load_unit_gen.hpp>
#include <bstream/iostream_bstream.hpp>
#include <bstream/vector_iobstream.hpp>
#include <fstream>
#include <iterator>
#include <util/path_util.hpp>

namespace mce {
namespace asset_gen {
uint64_t load_unit_gen::read_file_size(const std::string& path) {
	std::string sanitized_path = path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		uint64_t size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw std::runtime_error("Asset too big to fit in address space.");
		return size;
	} else
		throw std::runtime_error("Couldn't open asset file '" + path + "'.");
}
void load_unit_gen::compile_meta_data() {
	std::transform(entries.begin(), entries.end(), std::back_inserter(meta_data.assets),
				   [](const auto& entry) { return entry.meta_data; });
}
void load_unit_gen::copy_file_content(std::fstream& into, const load_unit_entry& entry) {
	std::string sanitized_path = entry.path;
	util::sanitize_path_inplace(sanitized_path);
	std::ifstream stream(sanitized_path, std::ios::binary);
	if(stream) {
		auto pos = into.tellp();
		if(pos < 0) throw std::runtime_error("Error getting write position for file '" + entry.path + "'.");
		if(uint64_t(pos) != entry.meta_data.offset)
			throw std::runtime_error("Offset mismatch for file '" + entry.path + "'.");
		constexpr size_t buffer_size = 1024;
		char buffer[buffer_size];
		uint64_t copied_bytes = 0;
		do {
			stream.read(buffer, buffer_size);
			into.write(buffer, stream.gcount());
			copied_bytes += stream.gcount();
		} while(stream.gcount());
		if(copied_bytes != entry.meta_data.size)
			throw std::runtime_error("Size mismatch for file '" + entry.path + "'.");
	} else
		throw std::runtime_error("Couldn't open asset file '" + entry.path + "'.");
}
void load_unit_gen::write_load_unit_metadata_file(const std::string& output_file) {
	std::fstream file_stream(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!file_stream) throw std::runtime_error("Can't open '" + output_file + "' for writing.");
	bstream::iostream_bstream stream(file_stream);
	stream << meta_data;
}
void load_unit_gen::write_load_unit_payload_file(const std::string& output_file) {
	std::fstream file_stream(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!file_stream) throw std::runtime_error("Can't open '" + output_file + "' for writing.");
	for(const auto& entry : entries) copy_file_content(file_stream, entry);
}
void load_unit_gen::add_file(const std::string& path, const std::string& name) {
	auto size = read_file_size(path);
	entries.emplace_back(path, name, next_pos, size);
	next_pos += size;
}
void load_unit_gen::compile_load_unit(const std::string& metadata_output_file,
									  const std::string& payload_output_file) {
	compile_meta_data();
	write_load_unit_metadata_file(metadata_output_file);
	write_load_unit_payload_file(payload_output_file);
}

} // namespace asset_gen
} // namespace mce
