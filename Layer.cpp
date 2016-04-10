#include "Layer.h"
#include "Utility.h"

Layer::Layer(){

}

Layer::Layer(int n):n(n){
	setSize(n);
}

Layer::~Layer(){

}
void Layer::setSize(int n){
	_I.set_size(n);
	_O.set_size(n);
	_G.set_size(n);
}

void Layer::transfer(vec v){
	_I.swap(v);
	//_O = _I;
	_O = sigmoid(_I);
	//cout << "I" << arma::size(_I) << endl;
	//cout << "O" << arma::size(_O) << endl;
//	_O.for_each([](mat::elem_type& val){val = sigmoid(val);});
	//return _O;
	//_I.for_each([]())
}
vec& Layer::I(){
	return _I;
}
vec& Layer::O(){
	return _O;
}
vec& Layer::G(){
	return _G;
}
