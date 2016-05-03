/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset_gen/load_unit_gen.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_GEN_HPP_
#define ASSET_GEN_LOAD_UNIT_GEN_HPP_

#include <cstdint>
#include <string>
#include <vector>
#include <asset/load_unit_meta_data.hpp>

namespace mce {
namespace asset_gen {

class load_unit_gen {
	struct load_unit_entry {
		std::string path;
		asset::asset_meta_data meta_data;
		load_unit_entry(const std::string& path, const std::string& name, uint64_t offset, uint64_t size)
				: path{path}, meta_data{offset, size, name} {}
	};
	std::vector<load_unit_entry> entries;
	uint64_t next_pos = 0;
	asset::load_unit_meta_data meta_data;

	static uint64_t read_file_size(const std::string& path);
	void compile_meta_data();
	void copy_file_content(std::fstream& into, const load_unit_entry& entry);
	void write_load_unit_metadata_file(const std::string& output_file);
	void write_load_unit_payload_file(const std::string& output_file);

public:
	void add_file(const std::string& path, const std::string& name);
	void compile_load_unit(const std::string& metadata_output_file, const std::string& payload_output_file);
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_GEN_HPP_ */
