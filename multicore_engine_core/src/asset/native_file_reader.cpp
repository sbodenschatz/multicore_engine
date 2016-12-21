/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/native_file_reader.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <asset/native_file_reader.hpp>
#include <exceptions.hpp>
#include <exceptions.hpp>
#include <fstream>
#include <util/path_util.hpp>

namespace mce {
namespace asset {

std::pair<file_content_ptr, file_size> native_file_reader::read_file(const std::string& prefix,
																	 const std::string& file) {
	std::string full_path = prefix;
	full_path += '/';
	full_path += file;
	util::sanitize_path_inplace(full_path);
	std::ifstream stream(full_path, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		file_size size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw buffer_size_exception("Asset too big to fit in address space.");
		stream.seekg(0, std::ios::beg);
		// TODO Maybe use 64-bit aligned storage for content
		std::shared_ptr<char> content =
				std::shared_ptr<char>(new char[size], [](char* ptr) { delete[] ptr; });
		stream.read(content.get(), size);
		stream.close();
		return std::make_pair(content, size);
	} else {
		return std::make_pair(file_content_ptr(), file_size(0ull));
	}
}

} // namespace asset
} // namespace mce
