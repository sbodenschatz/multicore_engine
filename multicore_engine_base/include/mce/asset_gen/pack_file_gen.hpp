/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset_gen/pack_file_gen.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_PACK_FILE_GEN_HPP_
#define ASSET_GEN_PACK_FILE_GEN_HPP_

/**
 * \file
 * Declares the pack_file_gen class containing the main functionality for pack file baking.
 */

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <mce/asset/pack_file_meta_data.hpp>
#include <string>
#include <utility>
#include <vector>

namespace mce {
namespace asset_gen {

/// The central class for providing the pack file generator functionality.
/**
 * An instance of this class holds the data needed for one pack file that is to be generated.
 * It saves the list of files to add to the pack and allows compiling the pack file after adding the files.
 * If another pack is to be generated a new instance of the class needs to be created.
 */
class pack_file_gen {
	struct pack_file_entry {
		std::string path;
		asset::pack_file_element_meta_data meta_data;
		asset::pack_file_element_meta_data orig_meta_data;
		int compression_level; //-2 no compression, -1 default compression, 0-9 low-high compression
		// cppcheck-suppress passedByValue
		pack_file_entry(std::string path, const std::string& name, uint64_t offset, uint64_t size)
				: path{std::move(path)}, meta_data{offset, size, 0ull, name},
				  orig_meta_data{offset, size, 0ull, name}, compression_level{-2} {}
		// cppcheck-suppress passedByValue
		pack_file_entry(std::string path, const std::string& name, uint64_t offset, uint64_t size,
						uint64_t compressed_size, int compression_level)
				: path{std::move(path)}, meta_data{offset, size, compressed_size, name},
				  orig_meta_data{offset, size, compressed_size, name}, compression_level{compression_level} {}
	};
	std::vector<pack_file_entry> entries;
	uint64_t next_pos = 0;
	uint64_t content_offset = 0;
	asset::pack_file_meta_data meta_data;
	std::vector<char> input_buffer;
	std::vector<char> compressed_buffer;

	static uint64_t read_file_size(const std::string& path);
	std::pair<uint64_t, uint64_t> read_file_size_compressed(const std::string& path, int level = -1);
	uint64_t calculate_meta_data_size() const;
	void compile_meta_data();
	void update_content_offset(uint64_t new_content_offset);
	void copy_file_content(std::fstream& into, const pack_file_entry& entry);
	void copy_file_content_compressed(std::fstream& into, const pack_file_entry& entry);
	void write_pack_file(const std::string& output_file);

public:
	/// Add a file to the prepared content of the pack file in uncompressed form.
	void add_file(const std::string& path, const std::string& name);
	/// Add a file to the prepared content of the pack file in compressed form with the given level.
	void add_file_compressed(const std::string& path, const std::string& name, int level = -1);
	/// Compile the prepared content into a pack file and write it to the given output file.
	void compile_pack_file(const std::string& output_file);
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_PACK_FILE_GEN_HPP_ */
