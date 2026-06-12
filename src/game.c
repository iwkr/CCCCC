#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "snl.h"
#include "tutils.h"
#include <unistd.h>

#define SQUARE_WIDTH_PTS 4
#define CHARS_PER_PT 4
#define TABLE_WIDTH_PTS (SQUARE_WIDTH_PTS*BOARD_WIDTH+1)
#define TABLE_WIDTH_CHARS (TABLE_WIDTH_PTS*CHARS_PER_PT)

static char topBorder[TABLE_WIDTH_CHARS] = BOX_DR;
static char midBorder[TABLE_WIDTH_CHARS] = BOX_UDR;
static char botBorder[TABLE_WIDTH_CHARS] = BOX_UR;
#define HZN BOX_RL BOX_RL BOX_RL BOX_RL
static void InitRenderer(){
	for (int k = 1;k<BOARD_WIDTH;k++){
		strcat(topBorder, HZN BOX_DRL);
		strcat(midBorder, HZN BOX_UDRL);
		strcat(botBorder, HZN BOX_URL);	
	}
	strcat(topBorder, HZN BOX_DL);
	strcat(midBorder, HZN BOX_UDL);
	strcat(botBorder, HZN BOX_UL);
}
static void RenderPlayerList(Game* game){
	printf("NEXT UP:\n");
	for (int i = 0; i<MAX_PLAYERS;++i){
		if(game->playerSlots[i] == 0) {
			continue;
		}
		// actual player
		bool isTurn = game->playerSlots[i] == game->turn;
		printf(isTurn?" [%c] ":"  %c  ", game->playerSlots[i]->avatar);
	}
	printf("\n");
}
//returns first char in input line, ignores the rest
static int GetLineChar(){
    int c = getchar();
    if (c == EOF) return EOF;
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;

    return c;
}

static void RenderMoveResult(MoveResult res){
	printf("Player (%c) rolled a %d", res.player->avatar, res.roll);
	if(res.bounce != 0){
		printf(", hit %d then bounced to %d",BOARD_LAST_IDX,res.bounce);
	}else{
		printf(", moved from %d to %d",res.start,res.end);
	}
	if(res.jumpOffset>0){
		printf("\nTHEN... a ladder took him");
		printf(G " %d " X,res.jumpOffset);
		printf("steps up\n");
	}else if (res.jumpOffset<0){
		printf("...\nBut a snake dragged him");
		printf(R " %d " X,abs(res.jumpOffset));
		printf("steps down\n");
	}else{
		printf("!\n");
	}
}
static void strapp(char* str, char c){
	int l = strlen(str);
	str[l] = c;
	str[l+1] = '\0';
}

static void RenderBoard(Game* game){
	bool ascending = true;
	printf("%s\n",topBorder);
	for(int i = BOARD_LAST_IDX-BOARD_WIDTH;i>1;i=i-BOARD_WIDTH){
		for(int j=0;j<BOARD_WIDTH;j++){
			int idx = ascending ? ++i : i--;
			int jump = game->jumpTable[idx];
			char playerString[MAX_PLAYERS*10] = ""; 
			// check if any players on this square
			for(int i = 0; i<MAX_PLAYERS; ++i){
				Player* player = game->playerSlots[i];
				if (player != NULL && player->position == idx){
					strapp(playerString,player->avatar);
				}
			}
			if (playerString[0] != '\0'){
				printf(BOX_UD BB "%-4s" X,playerString);
			} else if (jump>0){
				printf(jump>idx?(BOX_UD G "↑%02d " X ):
						(BOX_UD R "↓%02d " X ),
						jump);
			} else {
				printf(BOX_UD "%3d " ,idx);
			}	
		}
		printf(BOX_UD "\n%s\n",i>1?midBorder:botBorder);
		ascending = !ascending;
	}
}

// ~~ main
int main(){
	//init game
	uint32_t seed = time(NULL);
	InitRenderer();  
	Game game;
	InitGame(&game,seed);
	// init renderer
	// add players 
	Player players[MAX_PLAYERS];

	//for(int i = 0; i < 4; ++i){
	//	AddPlayer(&game, &players[i],'A'+i);
	//}
	printf("Number of players? [2-%d]->",MAX_PLAYERS);
	int numPlayersChoice = GetLineChar() - '0';
	while (numPlayersChoice<MIN_PLAYERS || numPlayersChoice>MAX_PLAYERS){
		printf("Please choose [2-%d]->",MAX_PLAYERS);
		numPlayersChoice = GetLineChar() - '0';
	}
	bool taken[128] = {0};
	for(int p = 0; p<numPlayersChoice; ++p){
		printf("Player %d symbol?->", p+1);
		int c = GetLineChar(); 
		while (!((c >= 33 && c <= 126) && !(c >= '0' && c <= '9')) || taken[c]){
			printf("Try something else ->"); 
			c = GetLineChar();
		}
		taken[c] = true;
		AddPlayer(&game, &players[p], c);
	}
	NextTurn(&game);
	// initial render before any move
	printf("\033[2;1H\033[J");
	printf("Starting game with seed: %u\n", seed);
	RenderBoard(&game);
	RenderPlayerList(&game);

	while (GetWinner(&game) == NULL) {
		printf("Press enter to roll...");
		while (getchar() != '\n');

		MoveResult moveResult = MakeMove(&game, GetRollDice(&game));
		NextTurn(&game);

		printf("\033[2;1H\033[J");
		int turn = GetTurn(&game);
		int round = GetRound(&game);
		printf("Starting game with seed: %u, turn %d, round %d\n",seed,turn,round);
		RenderBoard(&game);
		RenderMoveResult(moveResult);
		RenderPlayerList(&game);
	}

	printf("%c WON!\n", GetWinner(&game)->avatar);
};
