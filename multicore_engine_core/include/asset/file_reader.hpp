/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/file_reader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_FILE_READER_HPP_
#define ASSET_FILE_READER_HPP_

#include <memory>
#include <string>
#include <tuple>

namespace mce {
namespace asset {

class file_reader {
public:
	typedef std::shared_ptr<const char> file_content_ptr;
	typedef size_t file_size;

protected:
	static void sanitize_path_inplace(std::string& path) noexcept;

public:
	virtual ~file_reader() = default;
	virtual std::tuple<file_content_ptr, file_size> read_file(const std::string& prefix,
															  const std::string& file) = 0;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_FILE_READER_HPP_ */
