/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/file_reader.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_FILE_READER_HPP_
#define ASSET_FILE_READER_HPP_

#include <asset/asset_defs.hpp>
#include <string>
#include <utility>

namespace mce {
namespace asset {

/// Provides an interface used by asset loaders to access file through different mechanisms.
class file_reader {

public:
	/// Allows polymorphic destruction.
	virtual ~file_reader() = default;
	/// Hook function to read the given file into memory using the given prefix.
	virtual std::pair<file_content_ptr, file_size> read_file(const std::string& prefix,
															 const std::string& file) = 0;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_FILE_READER_HPP_ */
