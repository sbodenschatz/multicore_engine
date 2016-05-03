/*
 * Multi-Core Engine project
 * File /mutlicore_engine_load_unit_gen/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(disable : 4505)
#endif

#include <asset_gen/load_unit_gen.hpp>
#include <asset_gen/load_unit_description_parser.hpp>
#include <asset_gen/load_unit_description_ast.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <util/program_name.hpp>
#include <fstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
	std::string description_file;
	std::string payload_output_file;
	std::string metadata_output_file;
	// std::string deps_file;
	bool deps = false;

	po::options_description desc;
	desc.add_options()																			 //
			("help,h", "Display help message.")													 //
			("description-file,d", po::value(&description_file), "The description file to use.") //
			("payload-output,p", po::value(&payload_output_file),								 //
			 "The output file name for the payload data.")										 //
			("meta-output,m", po::value(&metadata_output_file),									 //
			 "The output file name for the metadata.")											 //
			("deps", po::bool_switch(&deps), "Only generate list of dependencies.")
			/*("deps", po::value(&deps_file),														 //
			 "Generate makefile-style dependency rules into given file")*/; //
	po::variables_map vars;
	po::store(po::parse_command_line(argc, argv, desc), vars);
	po::notify(vars);
	if(vars.count("help") || argc == 1) {
		std::cout << "Usage: " << mce::util::calculate_program_name(argv[0]) << " [options]" << std::endl;
		std::cout << desc;
		return -1;
	}
	if(!fs::exists(fs::path(description_file))) {
		std::cerr << "Description file '" << description_file << "' does not exist." << std::endl;
		return -3;
	}
	if(payload_output_file.empty()) {
		payload_output_file = fs::path(description_file).replace_extension("lup").string();
	}
	if(metadata_output_file.empty()) {
		metadata_output_file = fs::path(description_file).replace_extension("lum").string();
	}
	//	std::cout << description_file << std::endl;
	//	std::cout << payload_output_file << std::endl;
	//	std::cout << metadata_output_file << std::endl;
	mce::asset_gen::load_unit_gen gen;
	mce::asset_gen::parser::load_unit_description_parser parser;
	auto ast = parser.load_file(description_file);
	fs::path desc_dir = fs::path(description_file).parent_path();
	bool incomplete = false;
	for(const auto& section : ast) {
		for(const auto& entry : section.entries) {
			fs::path entry_path(entry.external_path);
			auto entry_path_abs = entry_path;
			if(entry_path.is_relative()) {
				if(entry.lookup == mce::asset_gen::ast::lookup_type::d) {
					entry_path_abs = fs::absolute(entry_path, desc_dir);
				} else {
					entry_path_abs = fs::absolute(entry_path, fs::current_path());
				}
			}
			if(!fs::exists(entry_path_abs)) {
				std::cerr << "File '" << entry_path << "' not found." << std::endl;
				incomplete = true;
				continue;
			}
			if(deps) {
				std::cout << entry_path_abs.generic_string() << ";";
			} else {
				auto internal_path = entry.internal_path;
				if(internal_path.empty()) {
					if(entry_path.is_relative()) {
						internal_path = entry.external_path;
					} else {
						// TODO: make entry path relative to desc file ?
						internal_path = entry_path.filename().string();
					}
				}
				gen.add_file(entry_path_abs.string(), internal_path);
			}
		}
	}
	std::cout.flush();
	if(!deps) gen.compile_load_unit(metadata_output_file, payload_output_file);
	if(incomplete) return -2;
}
