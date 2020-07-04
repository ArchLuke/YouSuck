#include "defs.h"

const int RookOpenFile = 15;
const int RookSemiOpenFile = 10;
const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25		
};

int EvalWhiteRook(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[wR]; ++pceNum) {
		sq = pos->pList[wR][pceNum];
		score += RookTable[SQ64(sq)];
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += RookOpenFile;
		} else if(!(pos->pawns[WHITE] & FileBBMask[FilesBrd[sq]])) {
			score += RookSemiOpenFile;
		}
	}	

}

int EvalBlackRook(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bR]; ++pceNum) {
		sq = pos->pList[bR][pceNum];
		score += RookTable[MIRROR64(SQ64(sq))];
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += RookOpenFile;
		} else if(!(pos->pawns[BLACK] & FileBBMask[FilesBrd[sq]])) {
			score += RookSemiOpenFile;
		}
	}	
	return -score;
}
