/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/pack_file_reader.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_PACK_FILE_READER_HPP_
#define ASSET_PACK_FILE_READER_HPP_

#include "file_reader.hpp"
#include <asset/pack_file_meta_data.hpp>
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <bstream/istream_bstream.hpp>
#include <fstream>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <util/lock_ptr_wrapper.hpp>

namespace mce {
namespace asset {

class pack_file_reader : public file_reader {
	struct pack_file_source {
		std::atomic_flag lock_flag = ATOMIC_FLAG_INIT;
		std::ifstream stream;
		pack_file_meta_data metadata;
		std::vector<char> compressed_buffer;
		std::vector<char> decompressed_buffer;
		pack_file_source(const std::string& pack_file_name) : stream{pack_file_name, std::ios::binary} {
			if(!stream) throw std::runtime_error("Unable to open file '" + pack_file_name + "'.");
			bstream::istream_bstream bstr(stream);
			bstr >> metadata;
			if(!bstr)
				throw std::runtime_error("Unable to read meta data from file '" + pack_file_name + "'.");
		}
		bool try_lock() {
			return !lock_flag.test_and_set();
		}
		void unlock() {
			lock_flag.clear();
		}
	};
	std::shared_timed_mutex sources_rw_lock;
	boost::container::flat_multimap<std::string, std::shared_ptr<pack_file_source>> opened_pack_file_sources;

	util::lock_ptr_wrapper<pack_file_source> get_source_stream(const std::string& prefix);

public:
	virtual std::pair<file_content_ptr, file_size> read_file(const std::string& prefix,
															 const std::string& file) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_PACK_FILE_READER_HPP_ */
