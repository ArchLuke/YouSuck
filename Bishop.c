#include "defs.h"
const int bishopMobility=3;
const int BishopPair=10;
const int bishopCenterControl=5;

//glboals
const int BiDir[4];
int EvalWhiteBishop(const S_BOARD *pos)
{
	int pceNum;
	int sq;
	int index;
	int t_sq;
	int score=0;
//loop through all the white bishops
	for(pceNum = 0; pceNum < pos->pceNum[wB]; ++pceNum) {
		sq = pos->pList[wB][pceNum];
		//loop through all the bishop move directions
		for(index = 0; index < 4; ++index) {
			int dir = BiDir[index];
			t_sq = sq + dir;
			//loop through all the moves in each direction
			while(SqOnBoard(t_sq)) {
				//if the bishop is controlling the center, add bonus
				if(t_sq == D4 || t_sq == D5 || t_sq==E4 || t_sq==E5)
				{
					score += bishopCenterControl;		
				}
				//if the bishop is behind a black pawn chain, stop going through this direction			
				if(pos->pieces[t_sq]==bP) 
				{
					if(pos->pieces[t_sq+11] == bP || pos->pieces[t_sq+9] == bP) {
						break;
					}	
				}
				//similarly to white pawn chains
				if(pos->pieces[t_sq]==wP) 
				{
					if(pos->pieces[t_sq-11] == wP || pos->pieces[t_sq-9] == wP) {
						break;
					}	
				}

				t_sq += dir;

				//add mobility score to the bishop
				score += bishopMobility;
			}
		}
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
	int t_sq;
	int score=0;
	for(pceNum = 0; pceNum < pos->pceNum[bB]; ++pceNum) {
		sq = pos->pList[bB][pceNum];
		for(index = 0; index < 4; ++index) {
			int dir = BiDir[index];
			t_sq = sq + dir;
			while(SqOnBoard(t_sq)) {
				if(t_sq == D4 || t_sq == D5 || t_sq==E4 || t_sq==E5)
				{
					score += bishopCenterControl;		
				}					
				if(pos->pieces[t_sq]==bP) 
				{
					if(pos->pieces[t_sq+11] == bP || pos->pieces[t_sq+9] == bP) {
						break;
					}	
				}
				if(pos->pieces[t_sq]==wP) 
				{
					if(pos->pieces[t_sq-11] == wP || pos->pieces[t_sq-9] == wP) {
						break;
					}	
				}

				t_sq += dir;
				score += bishopMobility;
			}
		}
	}
	if(pos->pceNum[bB]>1)
		score += BishopPair;

	return -score;	

}
