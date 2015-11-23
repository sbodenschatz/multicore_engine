/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/iobstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IOBSTREAM_HPP_
#define BSTREAM_IOBSTREAM_HPP_

namespace mce {
namespace bstream {

class iobstream : public ibstream, public obstream {
public:
	explicit operator bool() noexcept {
		return !errors();
	}

	bool errors() const noexcept {
		return eof() || invalid();
	}

	bool eof() const noexcept {
		return ibstream::eof() || obstream::eof();
	}

	bool invalid() const noexcept {
		return ibstream::invalid() || obstream::invalid();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IOBSTREAM_HPP_ */
