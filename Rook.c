#include "defs.h"
const int RookScore=3;
int EvalWhiteRook(const S_BOARD *pos)
{
	int horizontal;
	int vertical;
	int influence;
	int sq;
	int pceNum;
	int score=0;
	int blackKing=pos->pList[bK][0];
	int search=TRUE;
	for(pceNum = 0; pceNum < pos->pceNum[wR]; ++pceNum) {
		influence=0;
		sq = pos->pList[wR][pceNum];
		horizontal=FilesBrd[sq];
		vertical=RanksBrd[sq];
		while(++vertical<=RANK_8)
		{
	
			int sq64=8*vertical+horizontal;
			score += CheckKingSquare(pos,SQ120(sq64),blackKing, WHITE);
			if(!search)
				continue;
			if(pos->pieces[SQ120(sq64)] != wP && pos->pieces[SQ120(sq64)] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		vertical=RanksBrd[sq];
		search=TRUE;
		while(--vertical>=RANK_1)
		{
			int sq64=8*vertical+horizontal;
			score += CheckKingSquare(pos,SQ120(sq64),blackKing, WHITE);
			if(pos->pieces[SQ120(sq64)] != wP && pos->pieces[SQ120(sq64)] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		vertical=RanksBrd[sq];
		score += influence * RookScore;
		if(vertical==RANK_7)
			score += 5;

	}
	return score;
}
int EvalBlackRook(const S_BOARD *pos)
{
	int horizontal;
	int vertical;
	int influence;
	int sq;
	int score=0;
	int pceNum;
	int search=TRUE;
	int whiteKing=pos->pList[wK][0];
	for(pceNum = 0; pceNum < pos->pceNum[bR]; ++pceNum) {
		influence=0;
		sq = pos->pList[bR][pceNum];
		horizontal=FilesBrd[sq];
		vertical=RanksBrd[sq];
		while(++vertical<=RANK_8)
		{
			int sq64=8*vertical+horizontal;
			score -= CheckKingSquare(pos,SQ120(sq64),whiteKing, BLACK);
			if(!search)
				continue;
			if(pos->pieces[SQ120(sq64)] != wP && pos->pieces[SQ120(sq64)] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		vertical=RanksBrd[sq];
		search=TRUE;
		while(--vertical>=RANK_1)
		{	
			int sq64=8*vertical+horizontal;
			score -= CheckKingSquare(pos,SQ120(sq64),whiteKing,BLACK);
			if(!search)
				continue;
			if(pos->pieces[SQ120(sq64)] != wP && pos->pieces[SQ120(sq64)] !=bP )
				influence ++;
			else
				search=FALSE;
		}
		vertical=RanksBrd[sq];
		score -= influence * RookScore;
		if(vertical==RANK_2)
			score -= 5;

	}
	return score;
}
