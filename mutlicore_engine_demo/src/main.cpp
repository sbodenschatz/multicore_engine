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
}

