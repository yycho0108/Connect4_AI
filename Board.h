#ifndef __BOARD_H__
#define __BOARD_H__

#include <string.h>
#include <cassert>
#include <iostream>

#include "Utility.h"


enum Turn:char{X=0,A,B}; //X = empty
char cTurn[3] ={'X','A','B'};

template<int n, int m>
struct _Board{
	Turn turn;
	char _board[n][m];
	int _next[m]; //stores where to put next piece
	bool _open[m];

	Turn _win;
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
		_win = b._win;
		_reward = b._reward;
	}
	_Board(const _Board& b, int a)
	:_Board(b){ //b = prev
		step(a);
	}

	char* board(){
		return (char*)_board;
	}

	char* next(int a){
		return (char*)_next[a];
	}
	void step(int a){
		assert(_open[a] == true);
		namedPrint(a);
		namedPrint(_next[a]);
		_board[_next[a]][a] = turn;
		turn = (turn==A?B:A); //flip turn
		calcWin(a); //a = prev action
		calcNext();

		if(_win == turn) //may have to negate
			_reward = 1; //this is the previosu player's reward.
		else if (_win == X)
			_reward = 0; //draw
		else
			_reward = -1;
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
		if(_win == X){ //no one won yet
			_end = true;
			for(int a=0; a<m; ++a){
				_open[a] = false;
				for(int i=n-1; i>=0; --i){//bottom-up
					if (_board[i][a] == X){//empty
						_next[a] = i;
						_open[a] = true;
						_end = false;
						break;
					}
				}
			}
		}
	}
	bool inbound(int i, int j){
		return 0<=i && i<n && 0<=j && j<m;
	}
	void calcWin(int a){
		int c_i = _next[a];
		int c_j = a;
		_win = X; //no one won
		for(int di=-1;di<=1;++di){
			for(int dj=-1;dj<=1;++dj){ //eight directions (though up is redundnat)
				int ci = c_i;
				int cj = c_j;
				int cnt=0;
				if(di || dj){
					while(inbound(ci,cj) && _board[ci][cj] == turn){
						ci += di;
						cj += dj;
						++cnt;
					}
					if(cnt >= 4){
						_win = turn;
						return;
					}
				}
				
			}
		}
		//not implemented

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
