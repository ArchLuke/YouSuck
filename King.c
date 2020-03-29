#include "defs.h"
const int KingEndGame[64] = {	
	-50	,	-20	,	0	,	0	,	0	,	0	,	-20	,	-50	,
	-20,	0	,	20	,	20	,	20	,	20	,	0	,	-20	,
	0	,	20	,	30	,	30	,	30	,	30	,	20	,	0	,
	0	,	20	,	30	,	40	,	40	,	30	,	20	,	0	,
	0	,	20	,	30	,	40	,	40	,	30	,	20	,	0	,
	0	,	20	,	20	,	30	,	30	,	20	,	20	,	0	,
	-20	,	0	,	20	,	20	,	20	,	20	,	0	,	-20	,
	-50	,	-20	,	0	,	0	,	0	,	0	,	-20	,	-50	
};

const int KingMiddleGame[64] = {	
	0	,	20	,	20	,	-10	,	-10	,	0	,	20	,	5	,
	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,	-10	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-70	,	-70	,	-100	,	-100	,	-100	,	-100	,	-70	,	-70	,
	-70	,	-70	,	-100	,	-100	,	-100	,	-100	,	-70	,	-70	,
	-70	,	-70	,	-100	,	-100	,	-100	,	-100	,	-70	,	-70	,
	-70	,	-70	,	-100	,	-100	,	-100	,	-100	,	-70	,	-70	,
	-70	,	-70	,	-100	,	-100	,	-100	,	-100	,	-70	,	-70		
};
const int Mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};
const int KingsPawns=-20;
int EvalWhiteKing(const S_BOARD *pos)
{
	int sq = pos->pList[wK][0];
	int score=0;
	if(pos->material[BLACK] <= ENDGAME) {
		score += KingEndGame[SQ64(sq)];
	} else {
		score += KingMiddleGame[SQ64(sq)];
	}
	return score;
}
int EvalWhiteKingPawns(const S_BOARD *pos)
{
	int score=0;

	int square = pos->pList[wK][0];
	if(pos->material[BLACK] >= ENDGAME)
	{
		int sq64=SQ64(square);
		if(!(pos->pawns[WHITE] & (1ULL<<sq64+8))){
			score += KingsPawns;
		}
		if(FilesBrd[square] != FILE_H)
		{
			if(!(pos->pawns[WHITE] & (1ULL<<sq64+9))){
				score += KingsPawns;
			}
		}
		if((FilesBrd[square] != FILE_A))
		{
			if(!(pos->pawns[WHITE] & (1ULL<<sq64+7))){
				score += KingsPawns;
			}
		}
	}
	return score;
}
int EvalBlackKing(const S_BOARD *pos)
{
	int score=0;
	int square = pos->pList[bK][0];
	if((pos->material[WHITE] <= ENDGAME) ) {
		score -= KingEndGame[MIRROR64(SQ64(square))];
	} else {
		score -= KingMiddleGame[MIRROR64(SQ64(square))];
	}
	return score;
}
int EvalBlackKingPawns(const S_BOARD *pos)
{
	int score=0;
	int square = pos->pList[bK][0];
	if(pos->material[WHITE] >=ENDGAME)
	{
		int sq64=SQ64(square);
		if(!(pos->pawns[BLACK] & (1ULL<<sq64-8)))
			score -= KingsPawns;
		if(FilesBrd[square] != FILE_H)
		{
			if(!(pos->pawns[BLACK] & (1ULL<<sq64-7)))
				score -= KingsPawns;
		}
		if((FilesBrd[square] != FILE_A))
		{
			if(!(pos->pawns[BLACK] & (1ULL<<sq64-9)))
				score -= KingsPawns;
		}
	}
	return score;
}
