/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset_gen/load_unit_gen.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_LOAD_UNIT_GEN_HPP_
#define ASSET_GEN_LOAD_UNIT_GEN_HPP_

/**
 * \file
 * Declares the load_unit_gen class containing the main functionality for load unit baking.
 */

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <mce/asset/load_unit_meta_data.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {

/// The central class for providing the load unit generator functionality.
/**
 * An instance of this class holds the data needed for one load unit that is to be generated.
 * It saves the list of files to add to the unit and allows compiling the load unit after adding the files.
 * If another unit is to be generated a new instance of the class needs to be created.
 */
class load_unit_gen {
	struct load_unit_entry {
		std::string path;
		asset::asset_meta_data meta_data;
		// cppcheck-suppress passedByValue
		load_unit_entry(std::string path, const std::string& name, uint64_t offset, uint64_t size)
				: path{std::move(path)}, meta_data{offset, size, name} {}
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
	/// Add a file to the prepared content of the load unit.
	void add_file(const std::string& path, const std::string& name);
	/// Compile the prepared content into a load unit and write it to the given meta data and payload files.
	void compile_load_unit(const std::string& metadata_output_file, const std::string& payload_output_file);
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_LOAD_UNIT_GEN_HPP_ */
