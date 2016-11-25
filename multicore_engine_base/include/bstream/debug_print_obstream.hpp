/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/debug_print_obstream.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_
#define BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_

#include "obstream.hpp"
#include <ostream>

namespace mce {
namespace bstream {

class debug_print_obstream : public obstream {
	std::ostream& debug_stream;
	size_t write_position = 0;
	size_t written_size = 0;
	bool hex;

public:
	debug_print_obstream(std::ostream& debug_stream, bool hex = true);
	~debug_print_obstream() = default;
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_write() const noexcept override;
	virtual void seek_write(size_t position) override;
};

} /* namespace bstream */
} /* namespace mce */

#endif /* BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_ */
