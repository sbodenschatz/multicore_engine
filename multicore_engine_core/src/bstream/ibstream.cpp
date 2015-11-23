/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/ibstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <bstream/ibstream.hpp>

namespace mce {
namespace bstream {

ibstream& ibstream::operator>>(int8_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(int16_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(int32_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(int64_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(uint8_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(uint16_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(uint32_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(uint64_t& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(float& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(double& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}
ibstream& ibstream::operator>>(long double& value) {
	size_t size = read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
	if(size < sizeof(value)) { raise_read_eof(); }
	return *this;
}

} // namespace bstream
} // namespace mce
