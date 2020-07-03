#include "defs.h"
const int bishopMobility=3;
const int BishopPair=10;
const int BishopTable[64] = {
5	,	5	,	0	,	0	,	0	,	0	,	5	,	5	,
5	,	5	,	5	,	0	,	0	,	5	,	5	,	5	,
0	,	5	,	5	,	5	,	5	,	5	,	5	,	0	,
0	,	0	,	5	,	5	,	5	,	5	,	0	,	0	,
0	,	0	,	5	,	5	,	5	,	5	,	0	,	0	,
0	,	5	,	5	,	5	,	5	,	5	,	5	,	0	,
5	,	5	,	5	,	0	,	0	,	5	,	5	,	5	,
5	,	5	,	0	,	0	,	0	,	0	,	5	,	5	
};

//globals
U64 BlackBishopForwardMask[64][2];
int BlaxMaxDiagonalDistance[64][2];
int DiagonalDistance[64][64];
U64 WhiteBishopForwardMask[64][2];
int WhiteMaxDiagonalDistance[64][2];

int EvalWhiteBishop(const S_BOARD *pos)
{
	int pceNum;
	int sq;
	int index;
	int score=0;
//loop through all the white bishops
	for(pceNum = 0; pceNum < pos->pceNum[wB]; ++pceNum) {
		int mobility=7;
		sq = pos->pList[wB][pceNum];
		int sq64=SQ64(sq);
//the bishop should be on diagonals that control the center
		score += BishopTable[sq64];
		
		//check the diagonal influences
		U64 mask=WhiteBishopForwardMask[sq64][0] & pos->pawns[BOTH];	
		if(mask)
		{
			//int square=(63-__builtin_ctzll(mask));
			int square=FindBit(mask);
			mobility -= (WhiteMaxDiagonalDistance[sq64][0]-DiagonalDistance[sq][square]);
				
		}
 
		U64 mask2=WhiteBishopForwardMask[sq64][1] & pos->pawns[BOTH];		
		if(mask2)
		{
			//int square=(63-__builtin_ctzll(mask));
			int square=FindBit(mask2);
			mobility -= (WhiteMaxDiagonalDistance[sq64][1]-DiagonalDistance[sq64][square]);
		}
		score += mobility * bishopMobility;
		
				
	}
	//bishop pair
	if(pos->pceNum[wB]>1)
		score += BishopPair;

	return score;	
}
int EvalBlackBishop(const S_BOARD *pos)
{
	int pceNum;
	int sq;
	int index;
	int score=0;

//loop through all the black bishops
	for(pceNum = 0; pceNum < pos->pceNum[bB]; ++pceNum) {
		int mobility=7;
		sq = pos->pList[bB][pceNum];
		int sq64=SQ64(sq);
//the bishop should be on diagonals that control the center
		score += BishopTable[sq64];
		
		//check the diagonal influences
		U64 mask=BlackBishopForwardMask[sq64][0] & pos->pawns[BOTH];
		if(mask)
		{
		//	int square=FindBit(mask2);
			int square=(63-__builtin_clzll(mask));

			mobility -= (BlackMaxDiagonalDistance[sq64][0]-DiagonalDistance[square][sq64]);
				
		}

		U64 mask2=BlackBishopForwardMask[sq64][1] & pos->pawns[BOTH];		

		if(mask2)
		{
		//	int square=FindBit(mask2);
			int square=(63-__builtin_clzll(mask2));
			mobility -= (BlackMaxDiagonalDistance[sq64][1]-DiagonalDistance[square][sq64]);
				
		}

		score += mobility * bishopMobility;				
	}
	//bishop pair
	if(pos->pceNum[bB]>1)
		score += BishopPair;
	
	return -score;	

}
