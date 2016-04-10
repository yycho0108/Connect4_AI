#ifndef __AGENT_H__
#define __AGENT_H__
#include <deque>
#include "Net.h"
#include "Board.h"


//minimax Q-Learning
//possibly SARSA

template<int n, int m>
struct _Memory{
	using Board = _Board<n,m>;
	Board S;
	int a;
	int r;
	Board S2;
};

template<int n, int m>
class Agent{
	using Board = _Board<n,m>;
	using Memory = _Memory<n,m>;

	Board board;
	std::deque<Memory> memory;
	Net<n*m, m> net; //input = board-space, output = q value for next actions
	double gamma;

	Agent(){

	}

	void memorize(Board& S, int a, double r, Board& S2){
		memory.emplace_back(S,a,r,S2);
	}
	double max(Board& next){

		return 0.0;
	}

	void learn(Memory& mem, double alpha){
		auto x = std::vector<double>(mem.S.board(), mem.S.board()+n*m);
		auto y = net.FF(x);
		auto a = mem.a;
		auto r = mem.r;
		maxqn = max(mem.S2);

		y[a] = (1-alpha)*y[a] + alpha*(r+gamma*maxqn);
		// ...
		net.BP(y);
	}
	
};
#endif
