#include "Utility.h"
#include "Board.h"
#include "Agent.h"
#include <fstream>
#include <signal.h>

#define DIMS 5,5
//dimension of the board
//
static volatile bool run = true;

void stop(int){
	run=false;
}


void train(Agent<DIMS>& ai, int n){
	MiniMaxAgent<DIMS> m_ai(5);
	using Board = _Board<DIMS>;
	int epoch;
	Board board;
	int steps=0;
	const int u_freq = 1;
	const int n_update = 1;
	std::ofstream ferr("loss.csv");
	int* win = new int[n];

	for(epoch=0;run && epoch<n;++epoch){
		namedPrint(epoch);
		board = Board();

		double eps = 1.0 - tanh(2*float(epoch)/n); //gradual annealing
		//double eps = 1.0 - ((1.0-0.0)*epoch)/n; //linear annealing
		auto alpha = 0.4;
		auto AITURN = (split()<0.5)?A:B;
		do{
			int a;
			auto prev = board;//copy to prev
			//disable minimax agent for now
//			if(true){
//				a = ai.getNext(board,eps);
//				board.step(a);	
//			}

			if(board.turn == AITURN){
				//neural ai will play B (second)
				a = ai.getNext(board,eps);
				board.step(a);	
				//raw reward is the reward for the "next" player.
					//# memories to replay, learning rate
			}else{
				//minimax _ai will play B
				//or random ai will play B
				//auto a = r_ai.getBest(board);
				//a = ai.getNext(board,eps);
				a = m_ai.getBest(board);
				board.step(a);
			}

			ai.memorize(prev,a,-board.reward(),board);//hopefully this would make it learn
			//negate the reward, since reward should correspond to the previous state's action.
			//it works, since it's a zero-sum game.

			if(++steps && steps%u_freq == 0){
//				std::cout << "PREV  : " << std::endl;
//				prev.print();
//				std::cout << "NOW  : " << std::endl;
//				board.print();
//				namedPrint(board.reward());
				auto err = ai.learn(n_update, alpha); //300 = max of n_replay; 0.05 = learning rate
				ferr << err << endl;
				//std::cout << err << endl;
			}

		} while(!board.end());

			if(board._win == X){ //=draw
				win[epoch] = 0;
			}else if (board._win == AITURN){
				win[epoch] = 1;
			}else{
				win[epoch] = -1;
		// if B Wins A can't learn anything (because it's over on B's turn and a new game starts)
//testing AI purposes 
		}
//		board.print();
	}


	std::cout << "SAVING ... " << std::endl;
	std::ofstream ftrain("train.csv");
	for(int i=0;i<epoch;++i){
		ftrain << win[i] << std::endl;
	}
	ftrain.flush();
	ftrain.close();

	delete[] win;
}

void test(Agent<DIMS>& ai, int n){
	using Board = _Board<DIMS>;
	Board board;
	MiniMaxAgent<DIMS> m_ai(5);

	int* win = new int[n];

	int epoch;

	for(epoch=0;epoch<n;++epoch){//10 games
		board = Board();
		auto AITURN = (split()<0.5)?A:B;
		namedPrint(AITURN);
		do{
			board.print();
			int a;
			if(board.turn == AITURN){
				a = ai.getBest(board);
				board.step(a);
			}else{ //OPPONENT TURN
				a = m_ai.getBest(board);
				board.step(a);
			}
		}while(!board.end());

		if(board._win == X){ //=draw
			win[epoch] = 0;
		}else if (board._win == AITURN){
			win[epoch] = 1;
		}else{
			win[epoch] = -1;
		}
	}

	std::cout << "SAVING ... " << std::endl;
	std::ofstream ftest("test.csv");
	for(int i=0;i<epoch;++i){
		ftest<< (int)win[i] << std::endl;
	}
	ftest.flush();

	delete[] win;
}

int main(int argc, char* argv[]){
	signal(SIGINT,stop);

	int n = 1;
	if(argc != 1){
		n = std::atoi(argv[1]);
		// n = max_epoch
	}

	using Board = _Board<DIMS>;
	Board board;

	Agent<DIMS> ai(1,0.7);//memory size, gamma

	train(ai,n);

	test(ai,1000); //test for 1000 games

	//memory size = 1


	//ai.print();
	//test
	//board = Board();
	//int i = 0;
	//do{
	//	hline();
	//	board.print();
	//	auto a = ai.getNext(board,0.0);//no random exploration
	//	namedPrint(a);
	//	namedPrint(ai.guess(board));
	//	if((++i&1) == 0) //get user input
	//		std::cin >> a;
	//	board.step(a);

	//} while (!board.end());

}
