/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/native_file_reader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_NATIVE_FILE_READER_HPP_
#define ASSET_NATIVE_FILE_READER_HPP_

#include "file_reader.hpp"

namespace mce {
namespace asset {

class native_file_reader : public file_reader {
public:
	virtual std::pair<file_content_ptr, file_size> read_file(const std::string& prefix,
															 const std::string& file);
};

} // namespace asset
} // namespace mce

#endif /* ASSET_NATIVE_FILE_READER_HPP_ */
