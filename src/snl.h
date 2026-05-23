#ifndef SNL_H
#define SNL_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10
#define BOARD_SIZE (BOARD_WIDTH * BOARD_HEIGHT)
#define BOARD_LAST_IDX BOARD_SIZE
#define BOARD_START_IDX 1

#define MAX_PLAYERS 4
#define MIN_PLAYERS 2

typedef struct Game Game;
typedef struct Player Player;
typedef struct MoveResult MoveResult;
struct Player {
	int id; //Natural in [0,MAX_PLAYERS)
	int position; //Natural in [BOARD_START_IDX,BOARD_LAST_IDX]
	char avatar;
};

struct Game {
	//List of Player pointers (NULL if slot empty)
	Player* playerSlots[MAX_PLAYERS];
	int lastPlayerId;
	//result of last rand roll
	uint32_t randHead;
	//Isomorphic to board, one of:
	// 0  -> empty 
	// <0  -> jump target (ladder top/snake tail)
	// >0 -> jump start (value is target)  
	int jumpTable[BOARD_HEIGHT*BOARD_WIDTH+1];
	Player* turn;
};

struct MoveResult {
	//Player A rolled a 6 AND
	//Bounced back to xx
	//Hit a snake down to xx || Climbed a ladder up to xx
	Player* player;
	int roll;
	int start;
	int end;
	int bounce; 
	int jumpOffset;
};
void InitGame(Game* game,uint32_t seed);
bool AddPlayer(Game* game, Player* player, char avatar);
bool RemovePlayer(Game* game, Player* player);
bool NextTurn(Game* game);
int GetRollDice(Game* game);
MoveResult MakeMove(Game* game, int move);
Player* GetWinner(Game* game);
#endif
