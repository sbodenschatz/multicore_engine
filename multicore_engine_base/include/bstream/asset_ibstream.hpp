/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/asset_ibstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_ASSET_IBSTREAM_HPP_
#define BSTREAM_ASSET_IBSTREAM_HPP_

#include <bstream/ibstream.hpp>
#include <cstdint>
#include <memory>

namespace mce {
namespace asset {
class asset;
} // namespace asset
namespace bstream {

class asset_ibstream : public ibstream {
	std::shared_ptr<const asset::asset> asset;
	size_t read_position = 0;

public:
	asset_ibstream(std::shared_ptr<const asset::asset>  asset) : asset(std::move(asset)){};
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_read() const noexcept override;
	virtual void seek_read(size_t position) override;
	void reset() {
		clear_read_errors();
		read_position = 0;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_ASSET_IBSTREAM_HPP_ */
