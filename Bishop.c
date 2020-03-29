#include "defs.h"
const int BishopScore=3;
const int BishopPair=10;

int EvalWhiteBishop(const S_BOARD *pos)
{
	int influence;
	int sq;
	int pceNum;	
	int score=0;
	int blackKing=pos->pList[bK][0];
	int search=TRUE;
	for(pceNum = 0; pceNum < pos->pceNum[wB]; ++pceNum) {
		influence=0;
		sq = pos->pList[wB][pceNum];
		while(SqOnBoard(sq += 11))
		{
			score += CheckKingSquare(pos,sq, blackKing, WHITE);			
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		sq=pos->pList[wB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq-=11))
		{
			score += CheckKingSquare(pos,sq,blackKing,WHITE);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		sq=pos->pList[wB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq-=9))
		{
			score += CheckKingSquare(pos,sq,blackKing,WHITE);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;

		}
		sq=pos->pList[wB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq+=9))
		{
			score += CheckKingSquare(pos,sq,blackKing,WHITE);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;

		}

		score += influence * BishopScore;
	}
	if(pos->pceNum[wB]>1)
		score += BishopPair;

	return score;	
}
int EvalBlackBishop(const S_BOARD *pos)
{
	int influence;
	int sq;
	int pceNum;	
	int score=0;
	int whiteKing=pos->pList[wK][0];
	int search=TRUE;
	for(pceNum = 0; pceNum < pos->pceNum[wB]; ++pceNum) {
		influence=0;
		sq = pos->pList[bB][pceNum];
		while(SqOnBoard(sq += 11))
		{
			score -= CheckKingSquare(pos,sq, whiteKing,BLACK);			
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		sq=pos->pList[bB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq-=11))
		{
			score -= CheckKingSquare(pos,sq,whiteKing,BLACK);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		sq=pos->pList[bB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq-=9))
		{
			score -= CheckKingSquare(pos,sq,whiteKing,BLACK);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;

		}
		sq=pos->pList[bB][pceNum];
		search=TRUE;
		while(SqOnBoard(sq+=9))
		{
			score -= CheckKingSquare(pos,sq,whiteKing,BLACK);
			if(!search)
				continue;
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				search=FALSE;

		}
		score -= influence * BishopScore;
	}
	if(pos->pceNum[bB]>1)
		score -= BishopPair;

	return score;	
}
