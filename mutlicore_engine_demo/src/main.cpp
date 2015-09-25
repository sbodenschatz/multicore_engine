/*
 * Multi-Core Engine project
 * File /mutlicore_engine_demo/src/main.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <string>
#include <containers/unordered_object_pool.hpp>
#include <iostream>

struct X{
	int v;
	X(int v):v(v){
		std::cout<<"Constr. "<<v<<std::endl;
	}
	~X(){
		std::cout<<"Destr. "<<v<<std::endl;
	}
};

int main() {
	mce::containers::unordered_object_pool<X> uop;
	for(int i=0;i<2048;++i) uop.emplace(i);

	auto it = uop.cbegin();
	for(int i=0;i<100;++i){
		for(int j=0;j<i;++j){
			it++;
		}
		if(it==uop.cend()) break;
		it = uop.erase(it);
	}
	for(int i=8192;i<8300;++i) uop.emplace(i);

	auto it2=uop.begin();
	std::advance(it2,20);
	uop.erase(uop.begin(),it2);

	for(auto& v:uop){
		std::cout<<v.v<<std::endl;
	}

	uop.clear_and_reorganize();
	std::cout<<"Test"<<std::endl;
}

