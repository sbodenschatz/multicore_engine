/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/asset_ibstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <bstream/asset_ibstream.hpp>
#include <algorithm>
#include <cstring>
#include <asset/asset.hpp>
#include <stdexcept>

namespace mce {
namespace bstream {

size_t asset_ibstream::read_bytes(char* buffer, size_t count) noexcept {
	if(!asset->ready()) return 0u;
	if(read_position >= asset->size()) return 0u;
	size_t size = std::min(asset->size() - read_position, count);
	std::memcpy(buffer, asset->data() + read_position, size);
	read_position += size;
	return size;
}

size_t asset_ibstream::size() const noexcept {
	if(!asset->ready()) return 0u;
	return asset->size();
}
size_t asset_ibstream::tell_read() const noexcept {
	return read_position;
}
void asset_ibstream::seek_read(size_t position) {
	if(!asset->ready()) throw std::runtime_error("Asset not ready.");
	if(position <= asset->size()) {
		read_position = position;
	} else {
		throw std::runtime_error("Position outside of the referenced asset.");
	}
}

} // namespace bstream
} // namespace mce
