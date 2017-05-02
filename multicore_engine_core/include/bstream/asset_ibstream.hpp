/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/asset_ibstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_ASSET_IBSTREAM_HPP_
#define BSTREAM_ASSET_IBSTREAM_HPP_

/**
 * \file
 * Defines the binary stream interface for assets.
 */

#include <algorithm>
#include <bstream/ibstream.hpp>
#include <cstdint>
#include <memory>

namespace mce {
namespace asset {
class asset;
} // namespace asset
namespace bstream {

/// Provides a binary stream interface for assets for reading.
class asset_ibstream : public ibstream {
	std::shared_ptr<const asset::asset> asset;
	size_t read_position = 0;

public:
	/// Creates a binary stream interface for the given asset and participate in it's ownership.
	// cppcheck-suppress passedByValue
	explicit asset_ibstream(std::shared_ptr<const asset::asset> asset) : asset(std::move(asset)){};
	/// Reads bytes from the asset at the current read position.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	/// Returns the size of the asset.
	virtual size_t size() const noexcept override;
	/// Returns the current read position in the asset.
	virtual size_t tell_read() const noexcept override;
	/// Changes the current read position in the asset.
	virtual void seek_read(size_t position) override;
	/// Resets the state of the stream by clearing error flags and resetting the read position to 0.
	void reset() noexcept {
		clear_read_errors();
		read_position = 0;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_ASSET_IBSTREAM_HPP_ */
