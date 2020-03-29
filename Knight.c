#include "defs.h"
const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};


int EvalWhiteKnight(const S_BOARD *pos)
{
	int pceNum;
	int sq;
	int score=0;
	for(pceNum = 0; pceNum < pos->pceNum[wN]; ++pceNum) {
		sq = pos->pList[wN][pceNum];
		score += KnightTable[SQ64(sq)];
	}
	return score;
}
int EvalBlackKnight(const S_BOARD *pos)
{
	int pceNum;
	int sq;
	int score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bN]; ++pceNum) {
		sq = pos->pList[bN][pceNum];
		score -= KnightTable[MIRROR64(SQ64(sq))];
	}
	return score;
}
