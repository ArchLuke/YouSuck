#include "defs.h"
const int BishopScore=2.5;
int EvalWhiteBishop(const S_BOARD *pos)
{
	int influence;
	int sq;
	int pceNum;	
	int score=0;
	for(pceNum = 0; pceNum < pos->pceNum[wB]; ++pceNum) {
		influence=0;
		sq = pos->pList[wB][pceNum];
		sq += 11;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq += 11;
		}
		sq=pos->pList[wB][pceNum];
		sq -= 11;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq -= 11;
		}
		sq=pos->pList[wB][pceNum];
		sq -= 9;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq -= 9;
		}
		sq=pos->pList[wB][pceNum];
		sq += 9;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq += 9;
		}

		score += influence * BishopScore;

	}
	return score;	
}
int EvalBlackBishop(const S_BOARD *pos)
{
	int influence;
	int sq;
	int pceNum;
	int score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bB]; ++pceNum) {
		influence=0;
		sq = pos->pList[bB][pceNum];
		sq += 11;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq += 11;
		}
		sq=pos->pList[bB][pceNum];
		sq -= 11;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq -= 11;
		}
		sq=pos->pList[bB][pceNum];
		sq -= 9;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq -= 9;
		}
		sq=pos->pList[bB][pceNum];
		sq += 9;
		while(SqOnBoard(sq))
		{
			if(pos->pieces[sq] != wP && pos->pieces[sq] !=bP )
				influence ++;
			else
				break;
			sq += 9;
		}

		score -= influence * BishopScore;

	}	
	return score;
}
