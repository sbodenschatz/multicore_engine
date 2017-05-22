/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/obstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/bstream/obstream.hpp>

namespace mce {
namespace bstream {

obstream& obstream::operator<<(const int8_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int16_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int32_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const int64_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint8_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint16_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint32_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const uint64_t& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const float& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const double& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const long double& value) {
	if(!write_bytes(reinterpret_cast<const char*>(&value), sizeof(value))) raise_write_eof();
	return *this;
}
obstream& obstream::operator<<(const std::string& value) {
	uint64_t size = value.size();
	(*this) << size;
	for(const auto& entry : value) {
		uint8_t entry_uint8 = uint8_t(*reinterpret_cast<const unsigned char*>(&entry));
		if(static_cast<unsigned char>(entry_uint8) != *reinterpret_cast<const unsigned char*>(&entry))
			raise_write_invalid();
		(*this) << entry_uint8;
	}
	return *this;
}

} // namespace bstream
} // namespace mce
