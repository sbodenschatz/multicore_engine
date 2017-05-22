/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/util/program_name.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/util/program_name.hpp>
#include <algorithm>
#include <cctype>
#include <iterator>

namespace mce {
namespace util {

std::string calculate_program_name(const std::string& arg0) {
	std::string program_name = arg0;
	auto slash_pos = program_name.find_last_of("\\/");
	if(slash_pos != program_name.npos) program_name = program_name.substr(slash_pos + 1);
#ifdef MULTICORE_ENGINE_WINDOWS
	auto name_without_suffix_length = std::max(program_name.size(), std::string::size_type(4)) - 4;
	std::string suffix = ".exe";
	if(std::equal(program_name.begin() + name_without_suffix_length, program_name.end(), suffix.begin(),
				  suffix.end(), [](auto a, auto b) { return std::tolower(a) == std::tolower(b); })) {
		program_name = program_name.substr(0, name_without_suffix_length);
	}
#endif
	return program_name;
}

} // namespace util
} // namespace mce
