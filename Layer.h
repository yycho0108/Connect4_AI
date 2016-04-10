#ifndef __NET_H__
#define __NET_H__

#include "Utility.h"
#include "Layer.h"

#include <vector>
#include <armadillo>

#include <functional>
#include <ctime>
#include <random>

using namespace arma;

double randNum(){
	static auto _randNum = std::bind(std::uniform_real_distribution<double>(0.0,1.0),std::default_random_engine(time(0))); //random
	return _randNum();
}

void XOR_GEN(std::vector<double>& X, std::vector<double>& Y){
	X[0] = randNum()>0.5?1:0;
	X[1] = randNum()>0.5?1:0;
	Y[0] = int(X[0]) ^ int(X[1]);
}

template<int... Args>
class Net{
private:
	mat W[sizeof...(Args) - 1]; //Weights
	vec B[sizeof...(Args) - 1]; //Biases
	Layer L[sizeof...(Args)]; //Layers
	double alpha;
	double decay;
	int n;
public:

	Net(double alpha=0.6, double decay=0.001)
		:alpha(alpha),decay(decay),n(sizeof...(Args)){
		arma_rng::set_seed_random();
		init(Args...);
	}
	template<typename H1, typename H2>
	void init(H1 h1, H2 h2){
		int i = n-2; //sizeof...(t) = 0
		W[i].randn(h2,h1);
		B[i].randn(h2);
		L[i].setSize(h1);
		L[i+1].setSize(h2);
	}
	template<typename H1, typename H2, typename... T>
	void init(H1 h1, H2 h2, T... t){
		int i = n - (2 + sizeof...(t));
		W[i].randn(h2,h1);
		B[i].randn(h2);
		L[i].setSize(h1);
		init(h2,t...);
	}

	std::vector<double> FF(std::vector<double> X){
		L[0].O() = X;
		for(size_t i=1;i<n;++i){
			L[i].transfer(W[i-1]*L[i-1].O() + B[i-1]);	
		}
		return arma::conv_to<std::vector<double>>::from(L[n-1].O());
	}	
	void BP(std::vector<double> Y){
		//L[n-1].G() = vec(Y) - L[n-1].O();
		L[n-1].G() = vec(Y) - L[n-1].O();
		for(size_t i = n-2;i>=1;--i){
			L[i].G() = W[i].t() * L[i+1].G() % sigmoidPrime(L[i].O(),true);
		}
		for(size_t i=1;i<n;++i){
			//alpha = learning rate
			W[i-1] += alpha * (L[i].G() * L[i-1].O().t() - decay*W[i-1]);
			B[i-1] += alpha * (L[i].G() - decay*B[i-1]);
		}
	}
	void print(){
		for(int i=1;i<n;++i){
			W[i-1].print();
			B[i-1].print();
		}
	}

};
#endif
