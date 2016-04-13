#ifndef __AGENT_H__
#define __AGENT_H__
#include <deque>
#include <random>
#include <ctime>
#include "Net.h"
#include "Board.h"


//minimax Q-Learning
//possibly SARSA
float split(){
	return rand()/float(RAND_MAX);
}

constexpr long int ppow(int x, int n){ //positive power
	return n<1?1:x*ppow(x,n-1);
}

constexpr long int log2(long int x){
	return (x<2)?0:1+log2(x<<1);
}

template<int n, int m>
struct _Memory{
	using Board = _Board<n,m>;
	char S[n][m];
	int a;
	double r; //reward
	char S2[n][m];
	bool open[m]; //avaiable actions
	_Memory(Board& S, int a, double r, Board& S2):
		a(a),r(r){
			memcpy(this->S, S.board(), sizeof(this->S));
			memcpy(this->S2, S2.board(), sizeof(this->S2));
			memcpy(this->open, S2.open(), sizeof(this->open));
	}
};

template<int n, int m>
class Agent{
	using Board = _Board<n,m>;
	using Memory = _Memory<n,m>;

	std::deque<Memory> memories;
	Net<n*m,(int)log2(ppow(10,n+m)), m> net;
	//input = board-space, output = q value for next actions
	int mSize; //memory size
	double gamma;

public:
	Agent(int mSize=1, double gamma=0.8)
		:net(0.9,0.01, 0.0001), mSize(mSize),gamma(gamma){ 
			//small rho, eps, decay
			srand(time(0));
	}

	void memorize(Board& S, int a, double r, Board& S2){
		memories.emplace_back(S,a,r,S2);
		if(memories.size() > mSize){
			memories.pop_front();
		}
	}

	double max(char* next, bool* open){
		auto x = std::vector<double>(next, next+n*m);
		auto y = net.FF(x);	// list of q values for actions taken @ state 'next'
		//auto y = table.FF(x);
		double maxVal = 0.0;

		for(int a=0; a<m; ++a){
			//iterate over all possible actions
			if(open[a]){ //can perform action
				maxVal = maxVal>y[a]?maxVal:y[a];
			}
		}

		return -maxVal; //negate since it's max for opponent
		//still max since I'm assuming rational choice henceforth
	}

	double learn(Memory& mem, double alpha){
		//learn 1
		
		//input = current state
		auto x = std::vector<double>((char*)mem.S, (char*)mem.S+n*m);

		auto y = net.FF(x);
		//namedPrint(y);
		//auto y = table.FF(x);
		auto a = mem.a;
		auto r = mem.r;
		auto maxqn = max((char*)mem.S2, mem.open);

		y[a] = (1-alpha)*y[a] + alpha*(r+gamma*maxqn);

		//namedPrint(y);
		// ...
		net.BP(y);
		return net.error();
	}

	double learn(int n_replay, double alpha){
		//learn n
		auto s = memories.size();	
		auto avg = 0.0;
		for(int i=0;i<n_replay;++i){
			auto err = learn(memories[s*split()], alpha);
			avg += err;
		}
		return avg/n_replay;
	}

	int getRand(Board& board){
		const bool* open = board.open();
		std::vector<int> av;
		for(int a=0;a<m;++a){
			if(open[a])
				av.push_back(a);
		}

		return av[av.size()*split()];
	}
	int getBest(Board& board){
		auto x = std::vector<double>(board.board(),board.board()+n*m);
		//namedPrint(x);
		auto y = net.FF(x);
		//namedPrint(y);
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
		return (split()<eps)? getRand(board) : getBest(board);
	}
	std::vector<double> guess(Board& board){
		auto x = std::vector<double>(board.board(),board.board()+n*m);
		return net.FF(x);
	}
	void s_learn(Board& board, int action){// supervised learning
		auto x = std::vector<double>(board.board(), board.board()+n*m);
		auto y = net.FF(x);
		namedPrint(y);
		for(int i=0;i<y.size();++i){
			//augment "correct" action
			y[i] = (i==action)?1:0.8*y[i];
		}
		net.BP(y);
		//action = "correct" action
	}
	void print(){
		net.print();
	}
};

struct AR{
	int a; //action
	double r; //reward
};

template<int n, int m>
class MiniMaxAgent{
	using Board = _Board<n,m>;
private:
	int depth;
public:
	MiniMaxAgent(int depth)
		:depth(depth){

	}
	AR getBest(Board& board, int d, Turn turn){
		auto maximize = (turn == board.turn);
		AR res = {-1,0};
		if(d == 0 || board.end()){
			res.r = maximize? board.reward() : -board.reward();
		}
		else{
			res.r = maximize? -99999 : 99999;
			for(int a=0;a<m;++a){
				if(board._open[a]){
					Board b(board,a);
					auto ar = getBest(b, d-1, turn); //may have to negate here
					ar.r *= 0.8;//temporal reduction in reward

					if(maximize){
						if(ar.r>res.r){
							res.r = ar.r;
							res.a = a;
						}
					}
					else{
						if(ar.r<res.r){
							res.r = ar.r;
							res.a = a;
						}
					}
				}
			}

		}
		return res;
	}
	int getBest(Board& board){ //outputs best action, given current board state		
		return getBest(board,depth,board.turn).a;
	}

};

template<int n, int m>
class RandomAgent{
	using Board = _Board<n,m>;
public:
	RandomAgent(){}
	int getBest(Board& board){
		std::vector<int> v;
		auto open = board.open();
		for(int i=0;i<m;++i){
			if(open[i])
				v.push_back(i);
		}
		return v[v.size() * split()];
	}
};

#endif
