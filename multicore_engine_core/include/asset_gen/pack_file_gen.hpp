/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/pack_file_gen.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_GEN_HPP_
#define ASSET_GEN_PACK_FILE_GEN_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <asset/pack_file_meta_data.hpp>

namespace mce {
namespace asset_gen {

class pack_file_gen {
	struct pack_file_entry {
		std::string path;
		asset::pack_file_element_meta_data meta_data;
		asset::pack_file_element_meta_data orig_meta_data;
		pack_file_entry(const std::string& path, const std::string& name, uint64_t offset, uint64_t size)
				: path{path}, meta_data{offset, size, 0ull, name}, orig_meta_data{offset, size, 0ull, name} {}
	};
	std::vector<pack_file_entry> entries;
	uint64_t next_pos = 0;
	uint64_t content_offset = 0;
	asset::pack_file_meta_data meta_data;

	static uint64_t read_file_size(const std::string& path);
	uint64_t calculate_meta_data_size() const;
	void compile_meta_data();
	void update_content_offset(uint64_t new_content_offset);
	void copy_file_content(std::fstream& into, const pack_file_entry& entry);
	void write_pack_file(const std::string& output_file);

public:
	void add_file(const std::string& path, const std::string& name);
	void compile_pack_file(const std::string& output_file);
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_GEN_HPP_ */
