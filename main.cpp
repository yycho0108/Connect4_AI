#include "Utility.h"
#include "Board.h"
#include "Agent.h"
#include <fstream>
#include <signal.h>

static volatile bool run = true;

void stop(int){
	run=false;
}

int main(int argc, char* argv[]){
	signal(SIGINT,stop);

	int n = 1;
	if(argc != 1){
		n = std::atoi(argv[1]);
		// n = max_epoch
	}

	using Board = _Board<5,6>;
	Board board;

	Agent<5,6> ai(10000,0.8);//memory size, gamma
	MiniMaxAgent<5,6> m_ai(5);
	RandomAgent<5,6> r_ai;

	Turn win[n];

	const int u_freq = 4; // update frequency
	const int n_update = 32;

	//ai.print();
	int epoch;

	std::ofstream ferr("loss.csv");

	for(epoch=0;run && epoch<n;++epoch){
		namedPrint(epoch);
		board = Board();
		do{
			double eps = 1.0 - tanh(2*float(epoch)/n);
			auto alpha = 1.0; 
			if(board.turn == A){
				//neural ai will play A (first)
				//connect-four is solved to be a win for the first player
				auto a = ai.getNext(board,eps);
				auto prev = board; //copy to prev
				board.step(a);	
				ai.memorize(prev,a,-board.reward(),board);
				//raw reward is the reward for the "next" player.
				//negate the reward, since reward should correspond to the previous state's action.
				//it works, since it's a zero-sum game.
				
				if(epoch && epoch%u_freq == 0){
					auto err = ai.learn(n_update, alpha); //300 = max of n_replay; 0.05 = learning rate
					ferr << err << endl;
					//std::cout << err << endl;
				}
					//# memories to replay, learning rate
			}else{
				//minimax _ai will play B
				//or random ai will play B
				//auto a = r_ai.getBest(board);
				auto a = m_ai.getBest(board);
				board.step(a);
			}
		} while(!board.end());
		win[epoch] = board._win;
		board.print();
	}
	//ai.print();


	std::cout << "SAVING ... " << std::endl;
	std::ofstream f_win("win.csv");
	for(int i=0;i<epoch;++i){
		f_win << (int)win[i] << std::endl;
	}
	f_win.flush();
	f_win.close();

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
