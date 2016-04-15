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

	using Board = _Board<4,5>;
	Board board;

	Agent<4,5> ai(1000,0.9);//memory size, gamma
	MiniMaxAgent<4,5> m_ai(5);
	RandomAgent<4,5> r_ai;

	Turn win[n];

	const int u_freq = 20; // update frequency
	const int n_update = 400;

	//ai.print();
	int epoch;

	std::ofstream ferr("loss.csv");

	for(epoch=0;run && epoch<n;++epoch){
		namedPrint(epoch);
		board = Board();
		do{
			double eps = 1.0 - tanh(2*float(epoch)/n);
			auto alpha = 0.7;
			int a;
			auto prev = board;//copy to prev
			if(board.turn == A){
				//neural ai will play A (first)
				//connect-four is solved to be a win for the first player
				a = ai.getNext(board,eps);
				board.step(a);	
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
				a = m_ai.getBest(board);
				board.step(a);
			}
			ai.memorize(prev,a,-board.reward(),board);//hopefully this would make it learn
		} while(!board.end());
		// if B Wins A can't learn anything (because it's over on B's turn and a new game starts)

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
