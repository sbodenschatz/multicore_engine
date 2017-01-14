/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/iobstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IOBSTREAM_HPP_
#define BSTREAM_IOBSTREAM_HPP_

#include "ibstream.hpp"
#include "obstream.hpp"

namespace mce {
namespace bstream {

/// Provides a base class for binary streams that allow reading as well as writing.
class iobstream : public ibstream, public obstream {
public:
	/// Checks if the stream is in a valid state (no error flags are enabled, neither reading nor writing).
	explicit operator bool() noexcept {
		return !errors();
	}
	/// Checks if there are any errors (reading or writing).
	bool errors() const noexcept {
		return eof() || invalid();
	}
	/// Checks if any of the aspects (reading or writing) of the stream has the end of file flag set.
	bool eof() const noexcept {
		return ibstream::eof() || obstream::eof();
	}
	/// Checks if any of the aspects (reading or writing) of the stream has the invalid data flag set.
	bool invalid() const noexcept {
		return ibstream::invalid() || obstream::invalid();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IOBSTREAM_HPP_ */
