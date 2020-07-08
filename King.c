#include "defs.h"
//globals
int ColorSquare[64];
int KingManhattanDistances[64][64];

const int KingE[64] = {	
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10	,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10	,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	
};
/*
const int KingEd[64] = {	
	-50	,	-10	,	-5	,	5	,	40	,	20	,	10	,	10	,
	-30	,	-10	,	0	,	15	,	40	,	15	,	10	,	10	,
	-20	,	-5	,	5	,	20	,	35	,	15	,	5	,	-5	,
	-10	,	0	,	10	,	25	,	30	,	15	,	0	,	-10	,
	-10	,	0	,	15	,	30	,	25	,	10	,	0	,	-10	,
	-5	,	5	,	15	,	35	,	20	,	5	,	-5	,	-20	,
	10	,	10	,	15	,	40	,	15	,	0	,	-10	,	-30	,
	10	,	10	,	20	,	40	,	5	,	-5	,	-10	,	-50	
};

const int KingEl[64] = {	
	10	,	10	,	20	,	40	,	5	,	-5	,	-10	,	-50	,
	10	,	10	,	15	,	40	,	15	,	0	,	-10	,	-30	,
	-5	,	5	,	15	,	35	,	20	,	5	,	-5	,	-20	,
	-10	,	0	,	15	,	30	,	25	,	10	,	0	,	-10	,
	-10	,	0	,	10	,	25	,	30	,	15	,	0	,	-10	,
	-20	,	-5	,	5	,	20	,	35	,	15	,	5	,	-5	,
	-30	,	-10	,	0	,	15	,	40	,	15	,	10	,	10	,
	-50	,	-10	,	-5	,	5	,	40	,	20	,	10	,	10	
};
*/

const int KingO[64] = {	
	0	,	10	,	5	,	-20	,	-20	,	-10	,	10	,	5	,
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
	int material=pos->material[BLACK];
	if( (material >= ENDGAME_MAT) ) {
		score += KingO[MIRROR64(SQ64(sq))];
	} else{
		if(material > 0)
		{

			//int whiteMaterial=pos->material[WHITE];
			//bishop and knight mate deserves special attention*
			/*
			if(material==655 && whiteMaterial==0)
			{
				int square=pos->pList[bB][0];
				int color=ColorSquare[SQ64(square)];
				if(color)
				{
					score += 5*KingEd[SQ64(sq)];
				}
				else
				{
			score += 5*KingEl[SQ64(sq)];
			//	}
		}else
		{*/
			score += KingE[SQ64(sq)];
		//}
		}
		//piece checkmates require a slightly different king evaluation
		else
		{
			int blackKingSq=pos->KingSq[BLACK];
			score -= 2*KingManhattanDistances[SQ64(sq)][SQ64(blackKingSq)];

		}	
	}

	return score;	

}

int EvalBlackKing(const S_BOARD * pos)
{
	int score=0;
	int sq=pos->KingSq[BLACK];
	int material=pos->material[WHITE];
	if( (material >= ENDGAME_MAT) ) {
		score += KingO[MIRROR64(SQ64(sq))];
	} else{
		if(material > 0)
		{
		
		//	int blackMaterial=pos->material[BLACK];
			//bishop and knight mate deserves special attention
			/*
			if(material==655 && blackMaterial==0)
			{
				int square=pos->pList[wB][0];
				int color=ColorSquare[SQ64(square)];
				if(color)
					score += 5*KingEl[SQ64(sq)];
				else
					score += 5*KingEd[SQ64(sq)];
			}
			else
			{*/
			score += KingE[SQ64(sq)];
			//}
		}
		//piece checkmates require a slightly different king evaluation
		else
		{
			int whiteKingSq=pos->KingSq[WHITE];
			score -= 2*KingManhattanDistances[SQ64(sq)][SQ64(whiteKingSq)];

		}	
	}
	return -score;
}
