#ifndef __BOARD_H__
#define __BOARD_H__
enum Turn:char{O,X};

template<int n, int m>
struct _Board{
	Turn turn;
	char _board[n][m];
	_Board<n,m> _next[m];
	bool available[m];

	_Board(){
		calcNext();
	}

	char* board(){
		return _board;
	}
	_Board& next(int a){
		return _next[a];
	}
	void calcNext(){
		for(int a=0; a<m; ++a){
			auto& board = _next[a]._board;
			memcpy(board,_board,sizeof(_board));
			int i;

			for(i=n-1; i>=0; --i){//bottom-up
				if (board[i][a] == 0){
					board[i][a]= turn;
					break;
				}
			}

			if(i>=0){
				available[a] = true;
			}

		}
	}

};
#endif
