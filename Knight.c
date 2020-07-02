#include "defs.h"
/*evals*/
const int knightMobility=3;
const int outpost=15;
const int KnightTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
0	,	0	,	5	,	5	,	5	,	5	,	0	,	0	,
0	,	5	,	10	,	10	,	10	,	10	,	5	,	0	,
0	,	5	,	10	,	10	,	10	,	10	,	5	,	0	,
0	,	5	,	10	,	10	,	10	,	10	,	5	,	0	,
0	,	5	,	10	,	10	,	10	,	10	,	5	,	0	,
0	,	0	,	5	,	5	,	5	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};
//globals
U64 BlackKnightMobilityMask[64];
U64 BlackOutpostMask[64];
U64 BlackPawnSupportMask[64];
int KnightMobility[64];
U64 WhiteKnightMobilityMask[64];
U64 WhiteOutpostMask[64];
U64 WhitePawnSupportMask[64];

int EvalWhiteKnight(const S_BOARD *pos)
{

	int pceNum, index, sq, sq64, score=0;
	//loop through all the knights
	for(pceNum = 0; pceNum < pos->pceNum[wN]; ++pceNum) {
		sq = pos->pList[wN][pceNum];
		sq64=SQ64(sq);
//knight mobility
		//knight center control check
		score += KnightTable[sq64];
		//knight mobility check
		
		int squares=KnightMobility[sq64]-
			CountBits(WhiteKnightMobilityMask[sq64] & pos->pawns[BLACK]);
		score += (knightMobility*squares);
//knight outpost using bitmasks
		 //the knight needs to be in the enemy half of the board
		if(RanksBrd[sq]>RANK_4)
		{
			//no opposing black pawns to push the knight away, and the knight is supported by a white pawn
			if((pos->pawns[BLACK] & WhiteOutpostMask[sq64])==0 && 
			((WhitePawnSupportMask[sq64] & pos->pawns[WHITE]) != 0))
			{
				//add bonus to this knight for having an outpost
				score += outpost;
			}
		}

				
		
	}
	return score;
}
int EvalBlackKnight(const S_BOARD *pos)
{
	int pceNum, index, sq, sq64, score=0;
//loop through all the knights
	for(pceNum = 0; pceNum < pos->pceNum[bN]; ++pceNum) {
		sq = pos->pList[bN][pceNum];
		sq64=SQ64(sq);
//knight mobility
		//knight center control check
		score += KnightTable[sq64];
		//knight mobility check
		
		int squares=KnightMobility[sq64]-
			CountBits(BlackKnightMobilityMask[sq64] & pos->pawns[WHITE]);
		score += (knightMobility*squares);	
//knight outpost using bitmasks
		 //the knight needs to be in the enemy half of the board
		if(RanksBrd[sq]<RANK_5)
		{
			//no opposing black pawns to push the knight away, and the knight is supported by a white pawn
			if((pos->pawns[WHITE] & BlackOutpostMask[sq64])==0 && 
			((BlackPawnSupportMask[sq64] & pos->pawns[BLACK]) != 0))
			{
				//add bonus to this knight for having an outpost
				score += outpost;
			}
		}

				
		
	}
	return -score;
}
