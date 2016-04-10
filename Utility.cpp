#include "Utility.h"

double sigmoid(double x){
	return 1.0/(1.0 + exp(-x));
}
vec sigmoid(vec& v){
	return 1.0/(1.0 + exp(-v));
}
vec sigmoidPrime(vec& v, bool sig){
	if(sig)
		return v % (1.0-v);
	else{
		vec s = sigmoid(v);
		return s % (1.0-s);
	}
}

void hline(){
	cout << "-------------------------" << endl;
}

void checkPoint(std::string s){
	cout << "HERE : " << s <<  endl;
}
