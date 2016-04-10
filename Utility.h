#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <armadillo>
#include <vector>
#include <iostream>
using namespace arma;
double sigmoid(double);
vec sigmoid(vec&);
vec sigmoidPrime(vec&,bool);

template<typename T>
std::ostream& operator<<(std::ostream& o, std::vector<T> v){
	o << '[';
	for(auto& e : v){
		o << e << ", ";
	}
	o << ']';
	o << std::endl;
}

extern void hline();
extern void checkPoint(std::string s="");

#define namedPrint(x) \
	std::cout << #x << " : " << x << std::endl;
#endif
