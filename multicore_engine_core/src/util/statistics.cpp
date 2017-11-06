/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/statistics.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <boost/filesystem.hpp>
#include <fstream>
#include <mce/util/statistics.hpp>

namespace mce {
namespace util {

void statistics_manager::save(const char* separator) const {
	boost::filesystem::path sp("stats");
	if(!boost::filesystem::exists(sp)) {
		boost::filesystem::create_directory(sp);
	}
	std::shared_lock<std::shared_timed_mutex> lock(mtx);
	for(const auto& stat : stats_) {
		auto append = stat.second->append_output();
		boost::filesystem::path p = (sp / (stat.first + ".csv"));
		auto old_file = boost::filesystem::exists(p);
		std::ofstream fstr(p.generic_string(),
						   std::ios_base::out | (append ? std::ios_base::app : std::ios_base::trunc));
		stat.second->write_result_to(fstr, separator, stat.second->append_output() && old_file,
									 stat.second->append_output());
	}
}

void statistics_manager::clear_values() {
	std::unique_lock<std::shared_timed_mutex> lock(mtx);
	for(auto& stat : stats_) {
		stat.second->clear();
	}
}
void statistics_manager::clear() {
	std::unique_lock<std::shared_timed_mutex> lock(mtx);
	stats_.clear();
}

} // namespace util
} // namespace mce
