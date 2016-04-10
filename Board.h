#ifndef __BOARD_H__
#define __BOARD_H__

#include <string.h>
#include <cassert>
#include <iostream>

enum Turn:char{X=0,A,B}; //X = empty
char cTurn[3] ={'X','A','B'};

template<int n, int m>
struct _Board{
	Turn turn;
	char _board[n][m];
	char _next[m][n][m];
	bool _open[m];
	bool _end;
	double _reward;

	_Board(){
		turn = A;
		memset(_board,0,sizeof(_board));
		calcNext();
		_reward = 0.0;
	}
	_Board(const _Board& b){
		//copy
		turn = b.turn;
		memcpy(_board,b._board,sizeof(_board));
		memcpy(_next,b._next,sizeof(_next));
		memcpy(_open,b._open,sizeof(_open));
		_end = b._end;
		_reward = b._reward;
	}
	_Board(const _Board& b, int a){ //b = prev
		//assert(b._open[a] == true);
		turn = b.turn==A?B:A; //flip turn
		memcpy(_board,b.next(a),sizeof(_board));
		calcNext();

		_reward = _end? -1 : 0.1;
	}

	char* board(){
		return (char*)_board;
	}

	char* next(int a){
		return (char*)_next[a];
	}
	void step(int a){
		memcpy(_board,next(a),sizeof(_board));
		turn = turn==A?B:A; //flip turn
		calcNext();
		_reward = _end? -1 : 0.1;
	}

	bool end(){
		return _end;
	}

	double reward(){
		return _reward;
	}
	bool* open(){
		return _open;
	}

	void calcNext(){
		_end = true;
		for(int a=0; a<m; ++a){
			auto& board = _next[a];
			memcpy(board,_board,sizeof(_board));
			int i;

			for(i=n-1; i>=0; --i){//bottom-up
				if (board[i][a] == 0){
					board[i][a]= turn;
					break;
				}
			}

			if(i>=0){
				_end = false;
				_open[a] = true;
			}

		}
	}
	void print(){
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				putchar(cTurn[(int)_board[i][j]]);
			}
			putchar('\n');
		}
		std::cout << std::endl;
	}


};
#endif
