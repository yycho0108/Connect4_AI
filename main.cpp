#include "Utility.h"
#include "Board.h"
#include "Agent.h"
#include <fstream>


// test minimax agent

//int main(int argc, char* argv[]){
//
//	int n = 1;
//	if(argc != 1){
//		n = std::atoi(argv[1]);
//	}
//
//	using Board = _Board<6,7>;
//	Board board;
//	MiniMaxAgent<6,7> ai(7); //max_depth = 5
//
//	//test
//	board = Board();
//	int i = 0;
//	do{
//		hline();
//		board.print();
//		auto a = ai.getBest(board);//no random exploration
//		namedPrint(a);
//		if((++i&1) == 0) //get user input
//			std::cin >> a;
//		board.step(a);
//
//	} while (!board.end());
//
//}

// test supervised q-learning
int main(int argc, char* argv[]){

	int n = 1;
	if(argc != 1){
		n = std::atoi(argv[1]);
	}

	using Board = _Board<4,5>;
	Board board;

	Agent<4,5> ai(1000,0.6);//memory size, gamma
	MiniMaxAgent<4,5> m_ai(5);
	RandomAgent<4,5> r_ai;

	Turn win[n];

	//train ai with supervision from minimax agent
	for(int i=0;i<n;++i){
		namedPrint(i);
		board = Board();
		do{
			auto a = m_ai.getBest(board);
			ai.s_learn(board,a);
			board.step(a);
		} while(!board.end());
	}

	//testing
	for(int i=0;i<n;++i){
		namedPrint(i);
		board = Board();
		do{
			if(board.turn == B){
				//neural ai
				auto a = ai.getBest(board);
				board.step(a);	
			}else{
				//minimax ai
				auto a = m_ai.getBest(board);
				board.step(a);
			}
			board.print();
		} while(!board.end());
		win[i] = board._win;
		board.print();
	}
	std::ofstream f_win("win.csv");
	for(auto& w : win){
		f_win << (int)w << std::endl;
	}
	f_win.flush();
	f_win.close();
	
}
//int main(int argc, char* argv[]){
//
//	int n = 1;
//	if(argc != 1){
//		n = std::atoi(argv[1]);
//	}
//
//	using Board = _Board<4,5>;
//	Board board;
//
//	Agent<4,5> ai(1000,0.6);//memory size, gamma
//	MiniMaxAgent<4,5> m_ai(5);
//	RandomAgent<4,5> r_ai;
//
//	Turn win[n];
//
//	for(int i=0;i<n;++i){
//		namedPrint(i);
//		board = Board();
//		do{
//			double eps = 1.0 - tanh(2*float(i)/n);
//			if(board.turn == A){
//				//neural ai will play A
//				//connect-four is solved to be a win for the first player
//				auto a = ai.getNext(board,eps);
//				auto prev = board; //copy to prev
//				board.step(a);	
//				ai.memorize(prev,a,-board.reward(),board);
//				//raw reward is the reward for the "next" player.
//				//negate the reward, since reward should correspond to the previous state's action.
//				//it works, since it's a zero-sum game.
//				ai.learn(20, 0.05); //5 = max of n_replay; 0.05 = learning rate
//			}else{
//				//minimax _ai will play B
//				//or random ai will play B
//				//auto a = r_ai.getBest(board);
//				auto a = m_ai.getBest(board);
//				board.step(a);
//			}
//			board.print();
//		} while(!board.end());
//		win[i] = board._win;
//		board.print();
//	}
//	std::ofstream f_win("win.csv");
//	for(auto& w : win){
//		f_win << (int)w << std::endl;
//	}
//	f_win.flush();
//	f_win.close();
//	//test
//	//board = Board();
//	//int i = 0;
//	//do{
//	//	hline();
//	//	board.print();
//	//	auto a = ai.getNext(board,0.0);//no random exploration
//	//	namedPrint(a);
//	//	namedPrint(ai.guess(board));
//	//	if((++i&1) == 0) //get user input
//	//		std::cin >> a;
//	//	board.step(a);
//
//	//} while (!board.end());
//
//}
