#include "Utility.h"

double sigmoid(double x){
	return tanh(x);
	//return 1.0/(1.0 + exp(-x));
}
vec sigmoid(vec& v){
	return tanh(v);

	//return 1.0/(1.0 + exp(-v));
}
vec sigmoidPrime(vec& v, bool sig){
	if(sig){
		return 1.0 - (v%v);
	}else{
		vec s = sigmoid(v);
		return 1.0 - (s%s);
	}

//	if(sig)
//		return v % (1.0-v);
//	else{
//		vec s = sigmoid(v);
//		return s % (1.0-s);
//	}
}

void hline(){
	cout << "-------------------------" << endl;
}

void checkPoint(std::string s){
	cout << "HERE : " << s <<  endl;
}

double split(){
	static std::mt19937 gen(time(0));
	static std::uniform_real_distribution<double> dis(0.0, 1.0);
	return dis(gen);
	//return rand()/float(RAND_MAX);
}
