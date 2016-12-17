/*
 * Multi-Core Engine project
 * File /mutlicore_engine_load_unit_gen/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(disable : 4505)
#endif

#include <algorithm>
#include <asset_gen/obj_model_parser.hpp>
#include <asset_gen/static_model.hpp>
#include <asset_gen/static_model_exporter.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <cctype>
#include <core/version.hpp>
#include <fstream>
#include <iostream>
#include <model/model_format.hpp>
#include <string>
#include <util/program_name.hpp>
#include <util/string_tools.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace mce {
namespace model_converter {
enum class file_format { automatic, obj };
std::istream& operator>>(std::istream& in, file_format& fmt) {
	std::string token;
	in >> token;
	std::string orig_token;
	for(auto& c : token) {
		c = char(std::tolower(c));
	}
	if(token == "obj") {
		fmt = file_format::obj;
	} else {
		throw boost::program_options::invalid_option_value(orig_token);
	}
	return in;
}
std::ostream& operator<<(std::ostream& out, const file_format& fmt) {
	if(fmt == file_format::automatic) {
		out << "automatic";
	} else if(fmt == file_format::obj) {
		out << "obj";
	}
	return out;
}
}
}

int main(int argc, char* argv[]) {
	std::string model_output_file;
	std::string collision_output_file;
	std::string input_file;
	bool refs = false;
	mce::model_converter::file_format format = mce::model_converter::file_format::automatic;
	po::options_description desc;
	desc.add_options()																					   //
			("help,h", "Display help message.")															   //
			("version,v", "Display version info.")														   //
			("model,m", po::value(&model_output_file), "The output model file name.")					   //
			("collision,c", po::value(&collision_output_file), "The output collision geometry file name.") //
			("input,i", po::value(&input_file), "The input file name.")									   //
			("refs,r", po::bool_switch(&refs), "Only generate list of referenced files.")				   //
			("format,f", po::value(&format),															   //
			 "Override input format. \nSupported formats:\n"											   //
			 " obj - Wavefront OBJ")																	   //
			;																							   //

	po::variables_map vars;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vars);
	} catch(...) {
		std::cout << "Invalid arguments." << std::endl;
		argc = 1;
	}
	po::notify(vars);
	if(vars.count("help") || argc == 1) {
		std::cout << "Usage: " << mce::util::calculate_program_name(argv[0]) << " [options]" << std::endl;
		std::cout << desc;
		return -1;
	}
	if(vars.count("version")) {
		std::cout << "Multi-Core Engine project\n";
		std::cout << "model converter - Version " << mce::core::get_build_version_string() << "\n";
		std::cout << "Copyright 2015-2016 by Stefan Bodenschatz\n";
		std::cout << std::endl;
		return -1;
	}
	if(format == mce::model_converter::file_format::automatic) {
		if(mce::util::ends_with_ignore_case(input_file, "obj")) {
			format = mce::model_converter::file_format::obj;
		} else {
			std::cerr << "Could not determine input format for file name '" << input_file
					  << "' and no format given." << std::endl;
			return -2;
		}
	}
	if(!fs::exists(fs::path(input_file))) {
		std::cerr << "Input file '" << input_file << "' does not exist." << std::endl;
		return -3;
	}
	if(model_output_file.empty()) {
		model_output_file = fs::path(input_file).replace_extension("model").string();
	}
	if(collision_output_file.empty()) {
		collision_output_file = fs::path(input_file).replace_extension("col").string();
	}
	fs::path input_file_dir = fs::path(input_file).parent_path();
	mce::asset_gen::static_model model_data;
	mce::model::static_model_collision_data collision_data;
	std::vector<fs::path> refs_list;
	if(format == mce::model_converter::file_format::obj) {
		mce::asset_gen::obj_model_parser parser(input_file_dir);
		if(!refs) {
			parser.parse_file(input_file);
			std::tie(model_data, collision_data) = parser.finalize_model();
		} else {
			refs_list = parser.list_refs(input_file);
		}
	}
	if(!refs) {
		bool static_format = true;
		if(static_format) {
			mce::asset_gen::static_model_exporter exporter;
			exporter.export_model(model_data, model_output_file);
			exporter.export_model(collision_data, collision_output_file);
		}
	} else {
		for(auto& r : refs_list) {
			std::cout << r.generic_string() << ";";
		}
	}
}
