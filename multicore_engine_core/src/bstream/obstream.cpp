/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/obstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <bstream/obstream.hpp>

namespace mce {
namespace bstream {

obstream& obstream::operator<<(const int8_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int16_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int32_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int64_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint8_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint16_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint32_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint64_t& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const float& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const double& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const long double& value) {
	size_t size = write_bytes(reinterpret_cast<const char*>(&value), sizeof(value));
	if(size < sizeof(value)) raise_write_eof();
	return *this;
}

} // namespace bstream
} // namespace mce
