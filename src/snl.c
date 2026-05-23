#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "snl.h"
#include <assert.h>
#include "tutils.h"

#define NUM_SNAKES 5
#define NUM_LADDERS 5

#define SNAKE_MIN_LEN 10
#define LADDER_MIN_LEN 10
#define PORTAL_MIN_D 3 //min taxicab distance between two jumps
#define SNAKE_MIN_IDX BOARD_START_IDX
#define SNAKE_MAX_IDX (BOARD_LAST_IDX-1)
#define LADDER_MIN_IDX (BOARD_START_IDX+1)
#define LADDER_MAX_IDX (BOARD_LAST_IDX-1)

typedef struct {int x; int y;} Coords;

//dumbahh xorshift rand cus rand isn't isolated 
static uint32_t Roll32(Game* game){
	uint32_t x = game->randHead;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 5;
	game->randHead=x;
	return x;
}

// generate randon N in [a,b]
static int RollRange(Game* game,int a, int b){
	return (Roll32(game) % (b-a+1))+a; 
}

int GetRollDice(Game* game) {return RollRange(game,1,6);}

static Coords IdxToCoords(int idx){
	int nidx = idx-BOARD_START_IDX;
	int ny = nidx/BOARD_WIDTH;
	int nx = nidx-ny*BOARD_WIDTH;
	int y = BOARD_HEIGHT-ny;
	int x = y%2==0?BOARD_WIDTH-nx:nx+1;
	return (Coords){x,y};
}
static bool CoordsEq(Coords a, Coords b){ return a.x==b.x&&a.y==b.y; }

// get distance between two points
static int TaxiDistance(int idxa, int idxb){
	Coords ca = IdxToCoords(idxa);
	Coords cb = IdxToCoords(idxb);
	return abs(ca.x-cb.x)+abs(ca.y-cb.y);
};

// Generate snakes and ladders
static void GenJumpTable(Game* game){
	memset(game->jumpTable,0,sizeof(game->jumpTable));
	//Generation rules:
	int startIdx;
	int endIdx;
	int s = NUM_SNAKES;
	int l = NUM_LADDERS;
	int* jt = game->jumpTable;
	int placed[(NUM_SNAKES+NUM_LADDERS)*2];
	int nplaced = 0;
	while(s+l>0){
		bool makeSnake = (s>l);
		if (makeSnake){
			// gen snake candidate
			startIdx = RollRange(game,SNAKE_MIN_IDX+SNAKE_MIN_LEN,SNAKE_MAX_IDX);
			endIdx = RollRange(game,SNAKE_MIN_IDX,startIdx-SNAKE_MIN_LEN);
		} else {
			// gen ladder candidate
			// [2,98] [
			startIdx = RollRange(game,LADDER_MIN_IDX,LADDER_MAX_IDX-LADDER_MIN_LEN);
			endIdx = RollRange(game,startIdx+LADDER_MIN_LEN,LADDER_MAX_IDX);
		}
		
		//check start and end square is empty
		if (jt[startIdx] != 0 || jt[endIdx] != 0 ){
			goto retry;
		}
		for (int i=0;i<nplaced;++i){
			if (TaxiDistance(startIdx,placed[i]) < PORTAL_MIN_D){
				goto retry;
			}
		}

		
		jt[startIdx] = endIdx;
		jt[endIdx] = -startIdx;
		placed[nplaced] = startIdx;
		nplaced++;
		if (makeSnake) --s; else --l;
		
retry:;
	}
}
int getPlayerCount(Game* game){
	int c = 0;
	for (int i = 0; i<MAX_PLAYERS; ++i){
		if(game->playerSlots[i] != 0){
			++c;
		}
	}
	return c;
}
// initialize
void InitGame(Game* game, uint32_t seed){
	memset(game->playerSlots,0,sizeof(game->playerSlots));
	game->randHead = seed;
	game->lastPlayerId = 0;
	game->turn=0;
	game->currTurn=0;
	game->currRound=0;
	GenJumpTable(game);
}

// add a player
bool AddPlayer(Game* game, Player* player, char avatar){
	
	//search for empty slot
	int idx=-1;
	for (int i=0;i<MAX_PLAYERS;++i){
		if (game->playerSlots[i] == NULL) {
			idx=i;
			break;
		}
	}
	if (idx!=-1){
		player->position=1;
		player->avatar=avatar;
		player->id = ++game->lastPlayerId;
		game->playerSlots[idx] = player;
		return true;
	}else{
		return false;
	}

}

bool  RemovePlayer(Game* game, Player* player){
	int idx = -1;
	for (int i=0; i<MAX_PLAYERS; ++i){
		if (game->playerSlots[i] == player){
			idx=i;
			break;
		}
	}
	if(idx!=-1){
		game->playerSlots[idx] = NULL;
		return true;
	}else{
		return false;
	}
}
static bool NextRound(Game* game){
		// not yet set, first match
		game->currRound++;
		for(int i = 0;i<MAX_PLAYERS;++i){
			if(game->playerSlots[i]!=0){
				game->turn = game->playerSlots[i];
				return true;
			}
		}
}
bool NextTurn(Game* game){
	// if 0 players error out
	if (getPlayerCount(game) == 0){
		return false;
	}

	if (game->turn == 0){
		return NextRound(game);
	} else {
		game->currTurn++;
		// set, first match after curr
		int curri = 0;
	
		for (int i = 0; i<MAX_PLAYERS;++i){
			if (game->playerSlots[i]==game->turn){
				curri = i;
				break;
			}
		}
		//if currturn is last loop back
		if (curri == MAX_PLAYERS-1){ return NextRound(game);}
		//if not search
		for (int i = curri+1; i<MAX_PLAYERS; ++i){
			if(game->playerSlots[i]!=0){
				game->turn = game->playerSlots[i];
				return true;
			}
		}
		// if search exhajusts, start round
		return NextRound(game);
	}
}
//move current turn player
MoveResult MakeMove(Game* game, int move){
	MoveResult res;
	memset(&res,0,sizeof(res));
	res.roll = move;
	Player* pptr = game->turn;
	res.player = pptr;
	int startIdx = pptr->position;
	res.start=startIdx;
	int endIdx = startIdx + move;
	//actual end
	res.end = endIdx;
	if (endIdx > BOARD_LAST_IDX){
		endIdx = BOARD_LAST_IDX - (endIdx - BOARD_LAST_IDX);
		//after maybe bounce
		res.bounce = endIdx;
	}
	if (game->jumpTable[endIdx] > 0){
		endIdx = game->jumpTable[endIdx];
		res.jumpOffset=endIdx-res.end;
		//after maybe jump
		
	}
	pptr->position = endIdx;
	return res;
}

Player* GetWinner(Game* game){
	for(int i = 0; i < MAX_PLAYERS; ++i){
		Player* player = game->playerSlots[i];
		if (player != NULL && player->position == BOARD_LAST_IDX){
			return game->playerSlots[i];
		}
	}
	return NULL;
}

int GetRound(Game* game){
	return game->currRound;
}

int GetTurn(Game* game){
	return game->currTurn;
}
