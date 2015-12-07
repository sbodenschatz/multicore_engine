/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/native_file_reader.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/native_file_reader.hpp>
#include <fstream>

namespace mce {
namespace asset {

std::pair<file_content_ptr, file_size> native_file_reader::read_file(const std::string& prefix,
																	 const std::string& file) {
	std::string full_path = prefix;
	full_path += '/';
	full_path += file;
	sanitize_path_inplace(full_path);
	std::ifstream stream(full_path, std::ios::binary);
	if(stream) {
		stream.seekg(0, std::ios::end);
		auto size_tmp = stream.tellg();
		file_size size = size_tmp;
		decltype(size_tmp) size_check = size;
		if(size_check != size_tmp) throw std::runtime_error("Asset too big to fit in address space.");
		stream.seekg(0, std::ios::beg);
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
