/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/statistics.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <fstream>
#include <mce/util/statistics.hpp>

namespace mce {
namespace util {

void statistics_manager::save() const {
	std::shared_lock<std::shared_timed_mutex> lock(mtx);
	for(const auto& stat : stats_) {
		std::ofstream fstr(stat.first, std::ios_base::out | std::ios_base::trunc);
		stat.second->write_result_to(fstr);
	}
}

} // namespace util
} // namespace mce
