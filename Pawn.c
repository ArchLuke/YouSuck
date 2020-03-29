#include "defs.h"
const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-20	,	-20	,	0	,	10	,	10	,
15	,	0	,	0	,	5	,	5	,	0	,	0	,	15	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};
const int DoubledPawns=-10;
const int PawnBackwards= -10;
const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 };
const int PawnOverextended=-5;
int EvalWhitePawns(const S_BOARD *pos)
{
	int sqTable[64]={0};
	int score=0;
	int doubledPawns[64]={0};
	int sq;
	int pceNum;
	for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
		sq = pos->pList[wP][pceNum];
		sqTable[SQ64(sq)]=TRUE;
		
		score += PawnTable[SQ64(sq)];	
		
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			score += PawnIsolated;
		}
		if( (WhitePassedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			score += PawnPassed[RanksBrd[sq]];
		}
		
		if(WhiteBackwardsMask[SQ64(sq)] & pos->pawns[WHITE]==0)
		{
			PrintBitBoard(WhiteBackwardsMask[SQ64(sq)] & pos->pawns[WHITE]);
			score += PawnBackwards;
		}

	//	if((pos->pawns[BLACK] & (1ULL<<(sq-11))) || (pos->pawns[BLACK] & (1ULL<<(sq-9))) )
	//		score += PawnOverextended;

		int square;
		for(int i=RanksBrd[sq]+1;i<RANK_8;i++)
		{
			square=SQ64(sq+10*(i-RanksBrd[sq]));
			if (sqTable[square] && !doubledPawns[SQ64(sq)])
			{
				doubledPawns[square]=TRUE;
				score += DoubledPawns;
			}
		}
		for(int i=RanksBrd[sq]-1;i>RANK_1;i--)
		{
			int square=SQ64(sq-10*(RanksBrd[sq]-i));
			if (sqTable[square] && !doubledPawns[SQ64(sq)])
			{
				doubledPawns[square]=TRUE;
				score += DoubledPawns;
			}
		}	
	}
	return score;
}
int EvalBlackPawns(const S_BOARD *pos)
{
	int sqTable[64]={0};
	int doubledPawns[64]={0};
	int sq;
	int score=0;
	int pceNum;
	for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
		sq = pos->pList[bP][pceNum];
		sqTable[SQ64(sq)]=TRUE;
		score -= PawnTable[MIRROR64(SQ64(sq))];	
		
		if( (IsolatedMask[SQ64(sq)] & pos->pawns[BLACK]) == 0) {
			score -= PawnIsolated;
		}
		
		if( (BlackPassedMask[SQ64(sq)] & pos->pawns[WHITE]) == 0) {
			score -= PawnPassed[7 - RanksBrd[sq]];
		}

		if(BlackBackwardsMask[SQ64(sq)] & pos->pawns[BLACK]==0)
		{
			score -= PawnBackwards;
		}

	//	if((pos->pawns[WHITE] & (1ULL<<(sq+11))) || (pos->pawns[WHITE] & (1ULL<<(sq+9))) )
	//	{
	//		printf("called\n");
	//		score -= PawnOverextended;
	//	}
		int square;
		for(int i=RanksBrd[sq]+1;i<RANK_8;i++)
		{
			square=SQ64(sq+10*(i-RanksBrd[sq]));
			if (sqTable[square] && !doubledPawns[SQ64(sq)])
			{
				doubledPawns[square]=TRUE;
				score -= DoubledPawns;
			}
		}
		for(int i=RanksBrd[sq]-1;i>RANK_1;i--)
		{
			int square=SQ64(sq-10*(RanksBrd[sq]-i));
			if (sqTable[square] && !doubledPawns[SQ64(sq)])
			{
				doubledPawns[square]=TRUE;
				score -= DoubledPawns;
			}
		}
	}
	return score;	
}
