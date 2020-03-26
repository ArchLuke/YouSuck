// evaluate.c

#include "stdio.h"
#include "defs.h"

int EvalPosition(const S_BOARD *pos) {

	int pce;
	int pceNum;
	int sq;
	int score = pos->material[WHITE] - pos->material[BLACK];
	
	pce = wP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += PawnTable[SQ64(sq)];
	}	

	pce = bP;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= PawnTable[MIRROR64(SQ64(sq))];
	}	
	
	pce = wN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += KnightTable[SQ64(sq)];
	}	

	pce = bN;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= KnightTable[MIRROR64(SQ64(sq))];
	}			
	
	pce = wB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += BishopTable[SQ64(sq)];
	}	

	pce = bB;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= BishopTable[MIRROR64(SQ64(sq))];
	}	

	pce = wR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score += RookTable[SQ64(sq)];
	}	

	pce = bR;	
	for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
		sq = pos->pList[pce][pceNum];
		ASSERT(SqOnBoard(sq));
		score -= RookTable[MIRROR64(SQ64(sq))];
	}	
	
	if(pos->side == WHITE) {
		return score;
	} else {
		return -score;
	}	
}



















