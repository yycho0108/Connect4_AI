#include "Utility.h"
#include "Board.h"
#include "Agent.h"

int main(){
	using Board = _Board<3,3>;
	Board board;
	Agent<3,3> ai;
	int n = 1;

	for(int i=0;i<n;++i){
		namedPrint(i);
		board = Board();
		do{
			board.print();
			auto a = ai.getNext(board,0.05);
			auto prev = board; //copy to prev
			board.step(a);
			//maybe there's a better way of doing this?
			ai.memorize(prev,a,board.reward(),board);
			ai.learn(5, 0.05); //5 = max of n_replay

		} while(!board.end());
		//game over
	}

}
