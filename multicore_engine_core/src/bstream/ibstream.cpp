/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/ibstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <bstream/ibstream.hpp>
#include <cstring>

namespace mce {
namespace bstream {

ibstream& ibstream::operator>>(int8_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(int16_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(int32_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(int64_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(uint8_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(uint16_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(uint32_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(uint64_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(float& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(double& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}
ibstream& ibstream::operator>>(long double& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) {
		raise_read_eof();
	}
	return *this;
}

ibstream& ibstream::operator>>(std::string& value) {
	uint64_t size = 0;
	value.clear();
	(*this) >> size;
	if(size > value.max_size()) {
		raise_read_eof();
	} else {
		value.reserve(size_t(size));
		for(uint64_t i = 0; i < size; ++i) {
			uint8_t entry;
			(*this) >> entry;
			unsigned char entry_uchar = entry;
			value.push_back(*reinterpret_cast<const char*>(&entry_uchar));
		}
	}
	return *this;
}

} // namespace bstream
} // namespace mce
