/*
 * Multi-Core Engine project
 * File /mutlicore_engine_load_unit_gen/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(disable : 4505)
#endif

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <core/version.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <util/program_name.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
	po::options_description desc;
	desc.add_options()							   //
			("version,v", "Display version info.") //
			;									   //
	po::variables_map vars;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vars);
	} catch(...) {
		std::cout << "Invalid arguments." << std::endl;
		argc = 1;
	}
	po::notify(vars);
	if(vars.count("version")) {
		std::cout << "Multi-Core Engine project\n";
		std::cout << "model converter - Version " << mce::core::get_build_version_string() << "\n";
		std::cout << "Copyright 2015-2016 by Stefan Bodenschatz\n";
		std::cout << std::endl;
		return -1;
	}
}
