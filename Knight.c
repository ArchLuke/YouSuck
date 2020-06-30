#include "defs.h"
/*evals*/
const int knightMobility=3;
const int undefendedMinor=-5;
const int outpost=15;
const int knightCenterControl=5;

//globals
U64 BlackOutpostMask[64];
const int KiDir[8];
U64 WhiteOutpostMask[64];
int EvalWhiteKnight(const S_BOARD *pos)
{

	int pceNum;
	int index;
	int sq;
	int tempSq;
	int score=0;

	for(pceNum = 0; pceNum < pos->pceNum[wN]; ++pceNum) {
		sq = pos->pList[wN][pceNum];
//knight mobility
		for(index = 0; index < 8; ++index) {		
			tempSq = sq + KnDir[index];
			if(SqOnBoard(tempSq)) {
				if(tempSq == D4 || tempSq == D5 || tempSq==E4 || tempSq==E5)
				{
					score += knightCenterControl;		
				}	
				if(pos->pieces[tempSq+11] != bP && pos->pieces[tempSq+9] != bP)
					score += knightMobility; 
			}
		}

//undefended minor piece

		if(!SqAttacked(sq,WHITE,pos))
			score += undefendedMinor;
//knight outpost.
		 
		if(RanksBrd[sq]>RANK_4)
		{
			if((pos->pawns[BLACK] & WhiteOutpostMask[SQ64(sq)])==0 && 
			(pos->pieces[sq-9]==wP || pos->pieces[sq-11]==wP))
			{
				score += outpost;
			}
		}

				
		
	}
	return 1;
}
int EvalBlackKnight(const S_BOARD *pos)
{

	int pceNum;
	int index;
	int sq;
	int tempSq;
	int score=0;

	for(pceNum = 0; pceNum < pos->pceNum[bN]; ++pceNum) {
		sq = pos->pList[bN][pceNum];
//knight mobility
		for(index = 0; index < 8; ++index) {		
			tempSq = sq + KnDir[index];
			if(SqOnBoard(tempSq)) {
				if(tempSq == D4 || tempSq == D5 || tempSq==E4 || tempSq==E5)
				{
					score += knightCenterControl;		
				}	
				if(pos->pieces[tempSq-11] != wP && pos->pieces[tempSq-9] != wP)
					score += knightMobility; 
			}
		}

//undefended minor piece

		if(!SqAttacked(sq,BLACK,pos))
			score += undefendedMinor;
		if(RanksBrd[sq]<RANK_5)
		{
			if((pos->pawns[WHITE] & BlackOutpostMask[SQ64(sq)])==0 && 
			(pos->pieces[sq+9]==bP || pos->pieces[sq+11]==bP))
				score += outpost;
		}

			
	}
	return -1;
}
