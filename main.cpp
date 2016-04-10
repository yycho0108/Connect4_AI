#include "Utility.h"
#include "Board.h"
#include "Agent.h"

int main(int argc, char* argv[]){

	int n = 1;
	if(argc != 1){
		n = std::atoi(argv[1]);
	}

	using Board = _Board<4,5>;
	Board board;
	Agent<4,5> ai(1000,0.8);//memory size, gamma

	for(int i=0;i<n;++i){
		namedPrint(i);
		board = Board();
		do{
			double eps = 1.0 - tanh(2*float(i)/n);
			auto a = ai.getNext(board,eps);
			auto prev = board; //copy to prev
			board.step(a);
			
			ai.memorize(prev,a,-board.reward(),board);
			//raw reward is the reward for the "next" player.
			//negate the reward, since reward should correspond to the previous state's action.
			//it works, since it's a zero-sum game.
			
			ai.learn(20, 0.40); //5 = max of n_replay; 0.05 = learning rate

		} while(!board.end());
		board.print();
	}

	//test
	board = Board();
	int i = 0;
	do{
		hline();
		board.print();
		auto a = ai.getNext(board,0.0);//no random exploration
		namedPrint(a);
		namedPrint(ai.guess(board));
		if((++i&1) == 0) //get user input
			std::cin >> a;
		board.step(a);

	} while (!board.end());

}
