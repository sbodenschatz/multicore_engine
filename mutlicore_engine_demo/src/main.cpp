/*
 * Multi-Core Engine project
 * File /mutlicore_engine_demo/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <string>
#include <containers/unordered_object_pool.hpp>
#include <iostream>
#include <chrono>

struct X {
	int v;
	int dummy[127];
	X(int v) :
			v(v) {
//		std::cout<<"Constr. "<<v<<std::endl;
	}
	~X() {
//		std::cout<<"Destr. "<<v<<std::endl;
	}
};

void test_run(int runs, int objects, mce::containers::unordered_object_pool<X, 1024>& uop, int outer_run) {
	for(int run = 0; run < runs; ++run) {
		if(run % ((runs / 10) ? runs / 10 : 1) == 0) std::cout << outer_run << " " << run;
		for(int i = 0; i < objects - 1; ++i)
			uop.emplace(i);

		auto it = uop.cbegin();
		for(int i = 0; i < 100; ++i) {
			for(int j = 0; j < i; ++j) {
				it++;
			}
			if(it == uop.cend()) break;
			it = uop.erase(it);
		}
		for(int i = objects; i < objects + 100; ++i)
			uop.emplace(i);

		auto it2 = uop.begin();
		std::advance(it2, 20);
		uop.erase(uop.begin(), it2);

		if(run % ((runs / 10) ? runs / 10 : 1) == 0)
			std::cout << " " << uop.size() << " " << uop.capacity() << std::endl;
//		for (auto& v : uop) {
//				std::cout<<v.v<<std::endl;
//		}
	}
}

int main() {
	mce::containers::unordered_object_pool<X, 1024> uop;

	auto start = std::chrono::high_resolution_clock::now();
	for(int outer_run = 0; outer_run < 40; ++outer_run) {
		test_run(100, 0x10000, uop, outer_run);
		if(outer_run % 10) uop.clear();
		else uop.clear_and_reorganize();
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << "Test " << diff.count() << std::endl;
}

