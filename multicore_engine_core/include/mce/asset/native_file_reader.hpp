/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/native_file_reader.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_NATIVE_FILE_READER_HPP_
#define ASSET_NATIVE_FILE_READER_HPP_

/**
 * \file
 * Defines a file_reader that works directly from raw files.
 */

#include <mce/asset/asset_defs.hpp>
#include <mce/asset/file_reader.hpp>
#include <string>
#include <utility>

namespace mce {
namespace asset {

/// Loads files directly from the operating system file system.
class native_file_reader final : public file_reader {
public:
	/// Reads the given file from the given path prefix into memory.
	virtual std::pair<file_content_ptr, file_size> read_file(const std::string& prefix,
															 const std::string& file) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_NATIVE_FILE_READER_HPP_ */
