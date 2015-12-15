/*
 * Multi-Core Engine project
 * File /mutlicore_engine_load_unit_gen/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset_gen/load_unit_gen.hpp>
#include <asset_gen/load_unit_description_parser.hpp>
#include <asset_gen/load_unit_description_ast.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <util/program_name.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
	std::string description_file;
	std::string payload_output_file;
	std::string metadata_output_file;
	std::string deps_file;
	po::options_description desc;
	desc.add_options()																					  //
			("help,h", "Display help message.")															  //
			("description-file,d", po::value(&description_file), "The description file to use.")		  //
			("payload-output,p", po::value(&payload_output_file),										  //
			 "The output file name for the payload data.")												  //
			("meta-output,m", po::value(&metadata_output_file),											  //
			 "The output file name for the metadata.")													  //
			/*("deps", po::value(&deps_file),														 //
			 "Generate makefile-style dependency rules into given file")*/; //
	po::variables_map vars;
	po::store(po::parse_command_line(argc, argv, desc), vars);
	po::notify(vars);
	if(vars.count("help") || argc == 1) {
		std::cout << "Usage: " << mce::util::calculate_program_name(argv[0]) << " [options]" << std::endl;
		std::cout << desc;
	}
	if(payload_output_file.empty()) {
		payload_output_file = fs::path(description_file).replace_extension("lup").string();
	}
	if(metadata_output_file.empty()) {
		metadata_output_file = fs::path(description_file).replace_extension("lum").string();
	}
	std::cout << description_file << std::endl;
	std::cout << payload_output_file << std::endl;
	std::cout << metadata_output_file << std::endl;
	mce::asset_gen::load_unit_gen gen;
	mce::asset_gen::parser::load_unit_description_parser parser;
	auto ast = parser.load_file(description_file);
	fs::path desc_dir = fs::path(description_file).parent_path();
	for(const auto& section : ast) {
		for(const auto& entry : section.entries) {
			fs::path entry_path(entry.external_path);
			if(entry_path.is_relative()) {
				entry_path = fs::absolute(entry_path, desc_dir);
				gen.add_file(entry_path.string(), entry.internal_path);
			}
		}
	}
	gen.compile_load_unit(metadata_output_file, payload_output_file);
}
