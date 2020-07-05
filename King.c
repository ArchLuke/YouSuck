#include "defs.h"
const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};

const int KingO[64] = {	
	0	,	5	,	5	,	-10	,	-10	,	5	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70		
};

int EvalWhiteKing(const S_BOARD *pos)
{
	int score=0;
	int sq=pos->KingSq[WHITE];
	int sq64=SQ64(sq);
	int file=FileBrd[sq64];
	if( (pos->material[BLACK] <= ENDGAME_MAT) ) {
		score += KingE[SQ64(sq)];
	} else {
		score += KingO[SQ64(sq)];
			
	}
	

}

int EvalBlackKing(const S_BOARD * pos)
{
	int score=0;
	int sq=pos->KingSq[BLACK];
	int sq64=SQ64(sq);
	int file=FileBrd[sq64];

	if( (pos->material[WHITE] <= ENDGAME_MAT) ) {
		score -= KingE[MIRROR64(SQ64(sq))];
	} else {
		score -= KingO[MIRROR64(SQ64(sq))];
	}
}
