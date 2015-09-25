/*
 * Multi-Core Engine project
 * File /mutlicore_engine_demo/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <string>
#include <containers/unordered_object_pool.hpp>
#include <iostream>

int main() {
	mce::containers::unordered_object_pool<int> uop;
	mce::containers::unordered_object_pool<std::string> uop2;
	uop.insert(42);
	uop2.insert("Hallo Welt");
	std::cout<<"Test"<<std::endl;
	std::cout<<sizeof(*(uop.first_free_entry))<<" "<<sizeof(*(uop2.first_free_entry))<<std::endl;
	std::cout<<sizeof(int)<<" "<<sizeof(std::string)<<" "<<sizeof(size_t)<<" "<<sizeof(char*)<<std::endl;
}

