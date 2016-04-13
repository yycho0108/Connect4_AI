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
mat rms(mat& m, double eps){
	return sqrt(m+eps);
	//auto n = m.n_rows*m.n_cols;
	//return sqrt(arma::accu(m%m)/n) + eps;
}

mat lerp(mat a, mat b, double d){
	return d*a + (1-d)*b;
}

vec lerp(vec& a, vec b, double d){
	return d*a + (1-d)*b;
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
	mat egW[sizeof...(Args) - 1]; //gradient
	mat edW[sizeof...(Args) - 1]; //delta
	
	//mat g[sizeof...(Args) - 1]; //gain factor
	vec B[sizeof...(Args) - 1]; //Biases
	vec egB[sizeof...(Args) - 1]; //gradient
	vec edB[sizeof...(Args) - 1]; //delta
	Layer L[sizeof...(Args)]; //Layers
	double rho;// running average decay
	double eps;// small initial number
	double decay;// weight decay
	double loss;
	int n;
public:

	Net(double rho=0.99, double eps=0.0001, double decay=0.0001)
		:rho(rho),eps(eps),decay(decay),n(sizeof...(Args)){
		arma_rng::set_seed_random();
		init(Args...);
	}
	template<typename H1, typename H2>
	void init(H1 h1, H2 h2){
		int i = n-2; //sizeof...(t) = 0
		W[i].randu(h2,h1);
		egW[i].zeros(h2,h1); // set to 0
		edW[i].zeros(h2,h1); // set to 0

		//g[i].ones(h2,h1); // gain = 1

		B[i].randu(h2);
		egB[i].zeros(h2);
		edB[i].zeros(h2);
//		dB[i].zeros(h2);

		L[i].setSize(h1);
		L[i+1].setSize(h2);
	}
	template<typename H1, typename H2, typename... T>
	void init(H1 h1, H2 h2, T... t){
		int i = n - (2 + sizeof...(t));
		W[i].randu(h2,h1);
		egW[i].zeros(h2,h1); // set to 0
		edW[i].zeros(h2,h1); // set to 0
//		dW[i].randu(h2,h1);

		//g[i].ones(h2,h1);

		B[i].randu(h2);
		egB[i].zeros(h2);
		edB[i].zeros(h2);
//		dB[i].zeros(h2);

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

		loss = 0.5 * arma::dot(L[n-1].G(), L[n-1].G());

		for(size_t i = n-2;i>=1;--i){
			L[i].G() = W[i].t() * L[i+1].G() % sigmoidPrime(L[i].O(),true);
		}
		for(size_t i=1;i<n;++i){
			//alpha = learning rate

			mat gW = (L[i].G() * L[i-1].O().t()); //gradient
			egW[i-1] = lerp(egW[i-1], gW%gW, rho);
			mat scaler = rms(edW[i-1],eps)/rms(egW[i-1],eps);
			mat dW = rms(edW[i-1],eps)/rms(egW[i-1],eps) % gW;

			W[i-1] += dW - decay*W[i-1];//weight decay
			//-dW may be different sign
			edW[i-1] = lerp(edW[i-1],dW%dW,rho);

			//bias
			vec gB = L[i].G();
			egB[i-1] = lerp(egB[i-1], gB%gB, rho);
			vec dB = rms(edB[i-1],eps)/rms(egB[i-1],eps)%gB;
			
			B[i-1] += dB - decay*B[i-1];
			edB[i-1] = lerp(edB[i-1],dB%dB,rho);

		}
	}

	void save(std::string f){
		for(size_t i=0;i<n-1;++i){
			W[i].save(f + "_W_" + std::to_string(i));
		}

		for(size_t i=0;i<n-1;++i){
			B[i].save(f + "_B_" + std::to_string(i));
		}
	}
	void load(std::string f){
		for(size_t i=0;i<n-1;++i){
			W[i].load(f + "_W_" + std::to_string(i));
		}
		for(size_t i=0;i<n-1;++i){
			B[i].load(f + "_B_" + std::to_string(i));
		}
	}

	double error(){
		return loss;
	}

	void print(){
		for(int i=1;i<n;++i){
			W[i-1].print();
			B[i-1].print();
		}
	}

};
#endif
