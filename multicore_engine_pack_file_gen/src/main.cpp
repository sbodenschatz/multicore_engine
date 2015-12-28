/*
 * Multi-Core Engine project
 * File /mutlicore_engine_pack_file_gen/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <util/program_name.hpp>
#include <asset_gen/pack_file_gen.hpp>
#include <asset_gen/pack_file_description_parser.hpp>
#include <asset_gen/pack_file_description_ast.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
	std::string description_file;
	std::string output_file;
	std::string deps_file;
	po::options_description desc;
	desc.add_options()																			 //
			("help,h", "Display help message.")													 //
			("description-file,d", po::value(&description_file), "The description file to use.") //
			("output,o", po::value(&output_file), "The output file name.")						 //
			/*TODO:
			 * ("deps", po::value(&deps_file),														 //
			 "Generate makefile-style dependency rules into given file")*/;																			 //
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
	if(output_file.empty()) {
		output_file = fs::path(description_file).replace_extension(".pack").string();
	}
	std::cout << description_file << std::endl;
	std::cout << output_file << std::endl;
	mce::asset_gen::pack_file_gen gen;
	mce::asset_gen::parser::pack_file_description_parser parser;
	auto ast = parser.load_file(description_file);
	fs::path desc_dir = fs::path(description_file).parent_path();
	bool incomplete = false;
	for(const auto& section : ast) {
		for(const auto& entry : section.entries) {
			fs::path entry_path(entry.external_path);
			auto entry_path_abs = entry_path;
			if(entry_path.is_relative()) {
				entry_path_abs = fs::absolute(entry_path, desc_dir);
			}
			if(!fs::exists(entry_path_abs)) {
				std::cerr << "File '" << entry_path << "' not found." << std::endl;
				incomplete = true;
				continue;
			}
			auto internal_path = entry.internal_path;
			if(internal_path.empty()) {
				if(entry_path.is_relative()) {
					internal_path = entry.external_path;
				} else {
					// TODO: make entry path relative to desc file ?
					internal_path = entry_path.filename().string();
				}
			}
			if(section.zip_level > -2) {
				gen.add_file_compressed(entry_path_abs.string(), internal_path, section.zip_level);
			} else {
				gen.add_file(entry_path_abs.string(), internal_path);
			}
		}
	}
	gen.compile_pack_file(output_file);
	if(incomplete) return -2;
}
