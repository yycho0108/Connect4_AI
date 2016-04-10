#ifndef __AGENT_H__
#define __AGENT_H__
#include <deque>
#include <random>
#include <ctime>
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
	_Memory(Board& S, int a, int r, Board& S2):
		S(S),a(a),r(r),S2(S2){
	}
};

template<int n, int m>
class Agent{
	using Board = _Board<n,m>;
	using Memory = _Memory<n,m>;

	std::deque<Memory> memories;
	Net<n*m, m> net; //input = board-space, output = q value for next actions
	int mSize; //memory size
	double gamma;

public:
	Agent()
		:net(0.6,0.001){ //alpha, decay
			srand(time(0));
	}

	void memorize(Board& S, int a, double r, Board& S2){
		memories.emplace_back(S,a,r,S2);
		if(memories.size() > mSize){
			memories.pop_front();
		}
	}

	double max(Board& next){
		auto x = std::vector<double>(next.board(), next.board()+n*m);
		auto y = net.FF(x);	// list of q values for actions taken @ state 'next'
		//auto y = table.FF(x);
		double maxVal = 0.0;

		for(int a=0; a<m; ++a){
			if(next._open[a]){ //can perform action
				maxVal = maxVal>y[a]?maxVal:y[a];
			}
		}

		return -maxVal; //negate since it's max for opponent
		//still max since I'm assuming rational choice henceforth
	}

	void learn(Memory& mem, double alpha){
		//learn 1
		
		//input = current state
		auto x = std::vector<double>(mem.S.board(), mem.S.board()+n*m);

		auto y = net.FF(x);
		//auto y = table.FF(x);
		auto a = mem.a;
		auto r = mem.r;
		auto maxqn = max(mem.S2);

		y[a] = (1-alpha)*y[a] + alpha*(r+gamma*maxqn);
		// ...
		net.BP(y);
	}

	void learn(int n_replay, double alpha){
		//learn n
		auto s = memories.size();	
		for(int i=0;i<n_replay;++i){
			auto split = float(rand())/RAND_MAX;
			learn(memories[s*split], alpha);
		}
	}

	int getRand(Board& board){
		auto split = float(rand())/RAND_MAX;
		const bool* open = board.open();
		std::vector<int> av;
		for(int a=0;a<m;++a){
			if(open[a])
				av.push_back(a);
		}
		return av.size()*split;
	}
	int getBest(Board& board){
		auto x = std::vector<double>(board.board(),board.board()+n*m);
		auto y = net.FF(x);
		const bool* open = board.open();

		double maxVal = -99999;
		int maxAct = -1;

		for(int a=0;a<m;++a){
			if(open[a] && y[a] > maxVal){
				maxVal = y[a];
				maxAct = a;
			}
		}
		return maxAct;
	}
	int getNext(Board& board, double eps){
		auto split = float(rand())/RAND_MAX;
		return (split<eps)? getRand(board) : getBest(board);
	}
};
#endif
