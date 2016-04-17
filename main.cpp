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

void testNet(int argc, char* argv[]){
	int lim = 1000;
	if(argc != 1){
		lim = std::atoi(argv[1]);
	}
	Net<2,4,2> net(0.85,0.0001,0.0001); //for learning rate & decay use default
	std::vector<double> X(2);
	std::vector<double> Y(2);

	std::ofstream ferr("testNet.csv");

	auto start = clock();
	for(int i=0;i<lim;++i){
		XOR_GEN(X,Y);
		net.FF(X);
		net.BP(Y);
		ferr << net.error() << std::endl;
	}
	auto end = clock();
	printf("Took %f seconds", float(end-start)/CLOCKS_PER_SEC);
	for(int i=0;i<10;++i){
		XOR_GEN(X,Y);
		auto Yp = net.FF(X);
		std::cout << X[0] << ',' << X[1] << ':' <<  Y[0] << ',' << Y[1] << '|' << Yp[0] << ',' << Yp[1] << std::endl;
	}
}

void train(Agent<DIMS>& ai, int n){
	run = true;
	MiniMaxAgent<DIMS> m_ai(5);
	using Board = _Board<DIMS>;
	int epoch;
	Board board;
	int steps=0;

	const int u_freq = 1;
	const int n_update = 1;
	const int learn_start = 1;

	std::ofstream ferr("loss.csv");
	std::ofstream ftrain("train.csv");

	for(epoch=0;run && epoch<n;++epoch){
		namedPrint(epoch);
		board = Board();

		double eps = 1.0 - tanh(2*float(epoch)/n); //gradual annealing
		//double eps = 1.0 - ((1.0-0.0)*epoch)/n; //linear annealing
		auto alpha = 0.001;
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

			if(steps > learn_start && steps%u_freq == 0){
//				std::cout << "PREV  : " << std::endl;
//				prev.print();
//				std::cout << "NOW  : " << std::endl;
//				board.print();
//				namedPrint(board.reward());
				auto err = ai.learn(n_update, alpha); //300 = max of n_replay; 0.05 = learning rate
				ferr << err << endl;
				//std::cout << err << endl;
			}
			++steps;

		} while(!board.end());

		int win;

		if(board._win == X){ //=draw
			win = 0;
		}else if (board._win == AITURN){
			win = 1;
		}else{
			win = -1;
		}
		ftrain << win << std::endl;
	}

	ftrain.flush();
	ftrain.close();
}

void test(Agent<DIMS>& ai, int n){
	run = true;
	using Board = _Board<DIMS>;
	Board board;
	MiniMaxAgent<DIMS> m_ai(5);


	int epoch;
	std::ofstream ftest("test.csv");

	for(epoch=0;run && epoch<n;++epoch){//10 games
		board = Board();
		auto AITURN = (split()<0.5)?A:B;
		namedPrint(AITURN);
		do{
			board.print();
			int a;
			if(board.turn == AITURN){
				namedPrint(ai.guess(board));
				a = ai.getBest(board);
				board.step(a);
			}else{ //OPPONENT TURN
				a = m_ai.getBest(board);
				board.step(a);
			}
		}while(!board.end());

		int win;

		if(board._win == X){ //=draw
			win = 0;
		}else if (board._win == AITURN){
			win = 1;
		}else{
			win = -1;
		}
		ftest << win << std::endl;
	}

	ftest.flush();
	ftest.close();

}

int main(int argc, char* argv[]){
	//testNet(argc,argv);

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
