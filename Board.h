#ifndef __BOARD_H__
#define __BOARD_H__

#include <string.h>
#include <cassert>
#include <iostream>

#include "Utility.h"


enum Turn:char{A=-1,X,B}; //X = 0 = empty
char cTurn[3] ={'A','X','B'};

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

		_win = X;
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
		//assert(_open[a] == true);
		_board[_next[a]][a] = turn;
		calcWin(a); //a = prev action
		
		turn = (turn==A?B:A); //flip turn
		calcNext();

		if(_win == turn)//this would actually never happen.
			_reward = 1; //this is MY reward.
		else if (_win == X){
			_reward = 0; //draw
			//board-evaluation heuristic?
			for(int i=0;i<n;++i){
				for(int j=0;j<m;++j){
					if(_board[i][j] == turn){ //mine
						_reward += (n+m)/2 - (abs(i - n/2) + abs(j - m/2));
					}else if (_board[i][j] != X){ //opponent
						_reward -= (n+m)/2 - (abs(i - n/2) + abs(j - m/2));
					}
				}
			}

			//volume of pyramid = x*y*z/3
			_reward /= (n*m*((n+m)/4+1) /3) / 2; //volume of pyramid
			//divide by 2 since I can't have all grid
			//namedPrint(_reward);
		}
		else
			_reward = -1;
	}

	bool end(){
		//someone won or board is full
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
		if(_win == X){ //no one won yet
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
		int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
		for(int i=0;i<4;++i){
			auto di = dirs[i][0];
			auto dj = dirs[i][1];

			int cnt=0;

			int ci = c_i;
			int cj = c_j;

			while(inbound(ci,cj) && _board[ci][cj] == turn){
				ci -= di;
				cj -= dj;
				++cnt;
			}
			//back to center
			ci = c_i;
			cj = c_j;

			while(inbound(ci,cj) && _board[ci][cj] == turn){
				ci += di;
				cj += dj;
				++cnt;
			}

			--cnt;
			//counts the middle part twice
			
			if(cnt >= 4){ //connect-3 game right now
				_win = turn;
				return;
			}


		}
	}
	void print(){
		for(int i=0;i<n;++i){
			for(int j=0;j<m;++j){
				putchar(cTurn[(int)_board[i][j] + 1]);
			}
			putchar('\n');
		}
		std::cout << std::endl;
	}


};
#endif
