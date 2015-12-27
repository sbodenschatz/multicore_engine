/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/compression.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <util/compression.hpp>
#include <zlib.h>
#include <util/unused.hpp>
#include <cstring>
#include <algorithm>

// TODO Cite in sources
// Based on http://www.zlib.net/zlib_how.html

namespace mce {
namespace util {

std::vector<char> compress(const std::vector<char>& input, int level) {
	return compress(input, level, std::vector<char>());
}
std::vector<char> compress(const std::vector<char>& input, int level, std::vector<char>&& buffer) {
	std::vector<char> ret = std::move(buffer);
	using namespace zlib_wrappers;
	{
		zlib_deflate_stream strm(level);
		size_t remaining_input = input.size();
		size_t input_chunk_size = std::min(remaining_input, size_t(std::numeric_limits<unsigned int>::max()));
		const unsigned char* input_chunk_ptr = reinterpret_cast<const unsigned char*>(input.data());
		constexpr size_t output_chunk_size = 0x40000;
		flush_mode flush = flush_mode::no;
		do {
			char output_chunk[output_chunk_size];
			flush = remaining_input < size_t(std::numeric_limits<unsigned int>::max()) ? flush_mode::finish
																					   : flush_mode::no;
			strm.provide_input(input_chunk_ptr, input_chunk_size);
			do {
				strm.provide_output(reinterpret_cast<unsigned char*>(output_chunk), output_chunk_size);
				strm.deflate(flush);
				size_t have = output_chunk_size - strm.output_available();
				auto offset = ret.size();
				ret.resize(offset + have);
				std::memcpy(ret.data() + offset, output_chunk, have);
			} while(strm.output_available() == 0);

			remaining_input -= input_chunk_size;
			input_chunk_ptr += input_chunk_size;
			input_chunk_size = std::min(remaining_input, size_t(std::numeric_limits<unsigned int>::max()));
		} while(flush != flush_mode::finish);
	}
	return ret;
}
std::vector<char> decompress(const std::vector<char>& input) {
	return decompress(input, std::vector<char>());
}
std::vector<char> decompress(const std::vector<char>& input, std::vector<char>&& buffer) {
	std::vector<char> ret = std::move(buffer);
	using namespace zlib_wrappers;
	{
		zlib_inflate_stream strm;
		size_t remaining_input = input.size();
		size_t input_chunk_size = std::min(remaining_input, size_t(std::numeric_limits<unsigned int>::max()));
		const unsigned char* input_chunk_ptr = reinterpret_cast<const unsigned char*>(input.data());
		constexpr size_t output_chunk_size = 0x40000;
		return_code rc = return_code::ok;
		do {
			if(!remaining_input) break;
			char output_chunk[output_chunk_size];
			strm.provide_input(input_chunk_ptr, input_chunk_size);
			do {
				strm.provide_output(reinterpret_cast<unsigned char*>(output_chunk), output_chunk_size);
				rc = strm.inflate(flush_mode::no);
				if(rc == return_code::need_dict) zlib_check_error(return_code::error_data);
				size_t have = output_chunk_size - strm.output_available();
				auto offset = ret.size();
				ret.resize(offset + have);
				std::memcpy(ret.data() + offset, output_chunk, have);
			} while(strm.output_available() == 0);

			remaining_input -= input_chunk_size;
			input_chunk_ptr += input_chunk_size;
			input_chunk_size = std::min(remaining_input, size_t(std::numeric_limits<unsigned int>::max()));
		} while(rc != return_code::stream_end);
	}
	return ret;
}

} // namespace util
} // namespace mce
