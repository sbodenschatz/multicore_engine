/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/pack_file_reader.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef ASSET_PACK_FILE_READER_HPP_
#define ASSET_PACK_FILE_READER_HPP_

/**
 * \file
 * Defines a file_reader that reads from pack files.
 */

#include <mce/asset/asset_defs.hpp>
#include <mce/asset/file_reader.hpp>
#include <mce/asset/pack_file_meta_data.hpp>
#include <atomic>
#include <boost/container/flat_map.hpp>
#include <mce/bstream/istream_bstream.hpp>
#include <mce/exceptions.hpp>
#include <fstream>
#include <memory>
#include <shared_mutex>
#include <string>
#include <mce/util/lock_ptr_wrapper.hpp>
#include <utility>
#include <vector>

namespace mce {
namespace asset {

/// Loads files from within pack files that bundle together many asset files.
class pack_file_reader : public file_reader {
	struct pack_file_source {
		std::atomic_flag lock_flag = ATOMIC_FLAG_INIT;
		std::ifstream stream;
		pack_file_meta_data metadata;
		std::vector<char> compressed_buffer;
		std::vector<char> decompressed_buffer;
		explicit pack_file_source(const std::string& pack_file_name)
				: stream{pack_file_name, std::ios::binary} {
			if(!stream) throw path_not_found_exception("Unable to open file '" + pack_file_name + "'.");
			bstream::istream_bstream bstr(stream);
			bstr >> metadata;
			if(!bstr) throw io_exception("Unable to read meta data from file '" + pack_file_name + "'.");
		}
		bool try_lock() noexcept {
			return !lock_flag.test_and_set();
		}
		void unlock() noexcept {
			lock_flag.clear();
		}
	};
	std::shared_timed_mutex sources_rw_lock;
	boost::container::flat_multimap<std::string, std::shared_ptr<pack_file_source>> opened_pack_file_sources;

	util::lock_ptr_wrapper<pack_file_source> get_source_stream(const std::string& prefix);

public:
	/// Loads the given file from the pack file given in the prefix into memory.
	virtual std::pair<file_content_ptr, file_size> read_file(const std::string& prefix,
															 const std::string& file) override;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_PACK_FILE_READER_HPP_ */
