#include "defs.h"

const int QueenOpenFile = 10;
int EvalWhiteQueen(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[wQ]; ++pceNum) {
		sq = pos->pList[wQ][pceNum];
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += QueenOpenFile;
		} 
	}	
	return score;

}
int EvalBlackQueen(const S_BOARD *pos)
{
	int pceNum, sq, score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bQ]; ++pceNum) {
		sq = pos->pList[bQ][pceNum];
		if(!(pos->pawns[BOTH] & FileBBMask[FilesBrd[sq]])) {
			score += QueenOpenFile;
		} 
	}	
	return -score;
}
