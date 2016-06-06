/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/compression.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_COMPRESSION_HPP_
#define UTIL_COMPRESSION_HPP_

#include <cerrno>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

namespace mce {
namespace util {

std::vector<char> compress(const std::vector<char>& input, int level = -1);
void compress(const std::vector<char>& input, int level, std::vector<char>& out_buffer);
std::vector<char> decompress(const std::vector<char>& input);
void decompress(const std::vector<char>& input, std::vector<char>& out_buffer);

namespace zlib_wrappers {

enum class flush_mode {
	no = Z_NO_FLUSH,
	partial = Z_PARTIAL_FLUSH,
	sync = Z_SYNC_FLUSH,
	full = Z_FULL_FLUSH,
	finish = Z_FINISH,
	block = Z_BLOCK,
	trees = Z_TREES
};

enum class return_code {
	ok = Z_OK,
	stream_end = Z_STREAM_END,
	need_dict = Z_NEED_DICT,
	error_no = Z_ERRNO,
	error_stream = Z_STREAM_ERROR,
	error_data = Z_DATA_ERROR,
	error_mem = Z_MEM_ERROR,
	error_buf = Z_BUF_ERROR,
	error_version = Z_VERSION_ERROR
};

inline void zlib_check_error(return_code rc) {
	if(rc == return_code::error_no)
		throw std::runtime_error("ZLIB error number " + std::to_string(errno) + ".");
	if(rc == return_code::error_data) throw std::runtime_error("ZLIB data error.");
	if(rc == return_code::error_mem) throw std::runtime_error("ZLIB memory error.");
	if(rc == return_code::error_stream) throw std::runtime_error("ZLIB stream error.");
	if(rc == return_code::error_version) throw std::runtime_error("ZLIB version error.");
}

class zlib_deflate_stream {
	z_stream stream;

public:
	zlib_deflate_stream(int level = -1) {
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		return_code rc = static_cast<return_code>(deflateInit(&stream, level));
		zlib_check_error(rc);
	}
	~zlib_deflate_stream() {
		deflateEnd(&stream);
	}
	return_code deflate(flush_mode flush) {
		return_code rc = static_cast<return_code>(::deflate(&stream, static_cast<int>(flush)));
		zlib_check_error(rc);
		return rc;
	}
	void provide_input(const unsigned char* buffer, size_t size) {
		if(size > std::numeric_limits<unsigned int>::max()) {
			throw std::runtime_error("Buffer too big for ZLIB.");
		}
		stream.next_in = buffer;
		stream.avail_in = uInt(size);
	}
	void provide_output(unsigned char* buffer, size_t size) {
		if(size > std::numeric_limits<unsigned int>::max()) {
			throw std::runtime_error("Buffer too big for ZLIB.");
		}
		stream.next_out = buffer;
		stream.avail_out = uInt(size);
	}
	size_t input_available() const {
		return stream.avail_in;
	}
	size_t output_available() const {
		return stream.avail_out;
	}
};

class zlib_inflate_stream {
	z_stream stream;

public:
	zlib_inflate_stream() {
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		stream.avail_in = 0;
		stream.next_in = Z_NULL;
		return_code rc = static_cast<return_code>(inflateInit(&stream));
		zlib_check_error(rc);
	}
	~zlib_inflate_stream() {
		inflateEnd(&stream);
	}
	return_code inflate(flush_mode flush) {
		return_code rc = static_cast<return_code>(::inflate(&stream, static_cast<int>(flush)));
		zlib_check_error(rc);
		return rc;
	}
	void provide_input(const unsigned char* buffer, size_t size) {
		if(size > std::numeric_limits<unsigned int>::max()) {
			throw std::runtime_error("Buffer too big for ZLIB.");
		}
		stream.next_in = buffer;
		stream.avail_in = uInt(size);
	}
	void provide_output(unsigned char* buffer, size_t size) {
		if(size > std::numeric_limits<unsigned int>::max()) {
			throw std::runtime_error("Buffer too big for ZLIB.");
		}
		stream.next_out = buffer;
		stream.avail_out = uInt(size);
	}
	size_t input_available() const noexcept {
		return stream.avail_in;
	}
	size_t output_available() const noexcept {
		return stream.avail_out;
	}
};

} // namespace zlib_wrappers
} // namespace util
} // namespace mce

#endif /* UTIL_COMPRESSION_HPP_ */
