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
	int dummy[63];
	X(int v) :
			v(v) {
//		std::cout<<"Constr. "<<v<<std::endl;
	}
	~X() {
//		std::cout<<"Destr. "<<v<<std::endl;
	}
};

static const size_t block_size=1024;

void test_run(int runs, int objects, mce::containers::unordered_object_pool<X, block_size>& uop, int outer_run) {
	for(int run = 0; run < runs; ++run) {
		if(run % ((runs / 10) ? runs / 10 : 1) == 0) std::cout << outer_run << " " << run;
		for(int i = 0; i < objects; ++i)
			uop.emplace(i);

//		std::cout<<std::endl;
		auto& test_obj = *(uop.begin());
//		std::cout<<test_obj.v<<"###";
//		auto test_obj_it = uop.find(test_obj);
//		assert(test_obj_it==uop.begin());
		uop.find_and_erase(test_obj);

		const X* x = uop.insert(X(42424242));
		assert(x->v==42424242);

		{
			auto deleter = [&](X* d){
				uop.find_and_erase(*d);//Not optimal performance-wise
			};
			auto x2 = std::unique_ptr<X,decltype(deleter)>(uop.emplace(X(424242)),deleter);
			assert(x2->v==424242);
		}

//		for (auto& v : uop) {
//				std::cout<<v.v<<" ";
//		}
//		std::cout<<std::endl;

		auto it = uop.begin();
		for(int i = 0; i < 32; ++i) {
			if(it == uop.end()) break;
			++it;
			if(it == uop.end()) break;
			it = uop.erase(it);
		}
		uop.reorganize();
		//for(int i = objects; i < objects + 100; ++i)
		//	uop.emplace(i);

		//auto it2 = uop.begin();
		//std::advance(it2, 20);
		//uop.erase(uop.begin(), it2);

		if(run % ((runs / 10) ? runs / 10 : 1) == 0)
			std::cout << " " << uop.size() << " " << uop.capacity() << std::endl;
		for (auto& v : uop) {
				std::cout<<v.v<<std::endl;
		}
	}
}

int main() {
	mce::containers::unordered_object_pool<X, block_size> uop;

	auto start = std::chrono::high_resolution_clock::now();
	for(int outer_run = 0; outer_run < 1; ++outer_run) {
		test_run(1, 0x80, uop, outer_run);
		if(outer_run % 10) uop.clear();
		//else uop.clear_and_reorganize();
	}

	mce::containers::unordered_object_pool<X, block_size> uop3 = uop;
	mce::containers::unordered_object_pool<X, block_size> uop2 = std::move(uop);
	uop = uop3;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << "Test " << diff.count() << std::endl;
}

