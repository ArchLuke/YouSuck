#include "defs.h"
const int PawnDoubled=-10;
const int PawnIsolated = -15;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 

const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
0	,	0	,	0	,	-10	,	-10	,	0	,	0	,	0	,
0	,	5	,	5	,	10	,	10	,	5	,	5	,	0	,
0	,	5	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	5	,	10	,	20	,	20	,	10	,	5	,	5	,
10	,	10	,	20	,	25	,	25	,	20	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};
int EvalWhitePawns(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
		sq = pos->pList[wP][pceNum];
		score += PawnTable[SQ64(sq)];	
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			score += PawnIsolated;
		}
		
		if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			score += PawnPassed[RanksBrd[sq]];
		}
		if( (DoubledPawnsMask[SQ64(sq)] & pos->pawns[WHITE]) !=0) {
			score += PawnDoubled;
		}
			
	}
	return score;
}
int EvalBlackPawns(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
		sq = pos->pList[bP][pceNum];
	
		score += PawnTable[MIRROR64(SQ64(sq))];	
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			score += PawnIsolated;
		}
		
		if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			score += PawnPassed[7 - RanksBrd[sq]];
		}

		if( (DoubledPawnsMask[SQ64(sq)] & pos->pawns[BLACK]) !=0) {
			score += PawnDoubled;
		}
	}
	return -score;
}
