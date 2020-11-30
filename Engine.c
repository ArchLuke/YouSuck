#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

//global initializations
int BlackMaxDiagonalDistance[64][2];
int ColorSquare[64];
int DiagonalDistance[64][64];
int KingManhattanDistances[64][64];
int KnightMobility[64];
int WhiteMaxDiagonalDistance[64][2];

U64 BlackBishopForwardMask[64][2]={0};
U64 BlackKnightMobilityMask[64];
U64 BlackOutpostMask[64];
U64 BlackPassedMask[64];
U64 BlackPawnSupportMask[64];
U64 DoubledPawnsMask[64];
U64 FileBBMask[8];
U64 IsolatedMask[64];
U64 KnightMobilityMask[64];
U64 RankBBMask[8];
U64 WhiteBishopForwardMask[64][2]={0};
U64 WhiteKnightMobilityMask[64];
U64 WhiteOutpostMask[64];
U64 WhitePassedMask[64];
U64 WhitePawnSupportMask[64];

const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,    1, 10, -9, -11, 11, 9 };
const int KnDir[8] = { -8, -19,    -21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,    1, 10 };

int FilesBrd[BRD_SQ_NUM];
const int Mirror64[64] = {
56    ,    57    ,    58    ,    59    ,    60    ,    61    ,    62    ,    63    ,
48    ,    49    ,    50    ,    51    ,    52    ,    53    ,    54    ,    55    ,
40    ,    41    ,    42    ,    43    ,    44    ,    45    ,    46    ,    47    ,
32    ,    33    ,    34    ,    35    ,    36    ,    37    ,    38    ,    39    ,
24    ,    25    ,    26    ,    27    ,    28    ,    29    ,    30    ,    31    ,
16    ,    17    ,    18    ,    19    ,    20    ,    21    ,    22    ,    23    ,
8    ,    9    ,    10    ,    11    ,    12    ,    13    ,    14    ,    15    ,
0    ,    1    ,    2    ,    3    ,    4    ,    5,    6    ,    7
};
const int PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
const int PieceVal[13]= { 0, 100, 325, 330, 550, 1000, 50000, 100, 325, 330, 550, 1000, 50000  };
int RanksBrd[BRD_SQ_NUM];


//local variables
int ExpectedMove=NOMOVE;
int trapMove=NOMOVE;
U64 CastleKeys[16];
U64 PieceKeys[13][120];
U64 SideKey;

char *line;
const int TargetKing=5;
S_BOARD pos[1];
S_SEARCHINFO info[1];

const int Castle=5;
const int HashSize = 0x100000 * 160 ;

const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};
const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};
const int LoopNonSlideIndex[2] = { 0, 3 };
const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};
const int LoopSlideIndex[2] = { 0, 4 };
const int LoopSlidePce[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};

int MvvLvaScores[13][13];
const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

const int PceDir[13][8] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,    -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,    1, 10, 0, 0, 0, 0 },
    { -1, -10,    1, 10, -9, -11, 11, 9 },
    { -1, -10,    1, 10, -9, -11, 11, 9 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,    -21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,    1, 10, 0, 0, 0, 0 },
    { -1, -10,    1, 10, -9, -11, 11, 9 },
    { -1, -10,    1, 10, -9, -11, 11, 9 }
};
const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

const char FileChar[] = "abcdefgh";
const char PceChar[] = ".PNBRQKpnbrqk";
const char RankChar[] = "12345678";
const char SideChar[] = "wb-";

const int PieceBig[13] = { FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE };
const int PiecePawn[13] = { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE };

const int PieceBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE };
const int PieceKing[13] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE };
const int PieceKnight[13] = { FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE };
const int PieceRookQueen[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE };
const int PieceSlides[13] = { FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE };


static void AddCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
    list->count++;
}

static void AddEnPassantMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}


static void AddPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) {    
    int side=pos->side;
    if(RanksBrd[from] == (side?RANK_2:RANK_7)) {
        AddCaptureMove(pos, MOVE(from,to,cap,(side?bQ:wQ),0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,(side?bR:wR),0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,(side?bB:wB),0), list);
        AddCaptureMove(pos, MOVE(from,to,cap,(side?bN:wN),0), list);
    } else {
        AddCaptureMove(pos, MOVE(from,to,cap,EMPTY,0), list);
    }
}
static void AddPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list ) {
    int side=pos->side;
    if(RanksBrd[from] == (side?RANK_2:RANK_7)) {
        AddQuietMove(pos, MOVE(from,to,EMPTY,(side?bQ:wQ),0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,(side?bR:wR),0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,(side?bB:wB),0), list);
        AddQuietMove(pos, MOVE(from,to,EMPTY,(side?bN:wN),0), list);
    } else {
        AddQuietMove(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
    }
}
static void AddPiece(const int sq, S_BOARD *pos, const int pce) {

    
    int col = PieceCol[pce];

        HASH_PCE(pce,sq);
    
    pos->pieces[sq] = pce;

        if(PieceBig[pce]) {
        pos->bigPce[col]++;
    
    } else {
        SETBIT(pos->pawns[col],SQ64(sq));
        SETBIT(pos->pawns[BOTH],SQ64(sq));
    }
    
    pos->material[col] += PieceVal[pce];
    pos->pList[pce][pos->pceNum[pce]++] = sq;
    
}
static void AddQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list ) {

    list->moves[list->count].move = move;
    
    if(pos->searchKillers[0][pos->ply] == move) {    
        list->moves[list->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {    
        list->moves[list->count].score = 800000;
    } else {    
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }
    list->count++;
}
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull, S_MOVELIST *list, int listProvided) {
    if(depth == 0) {
        return Quiescence(alpha, beta, pos, info);
        // return EvalPosition(pos);
    }
    
    if(( info->nodes & 2047 ) == 0) {
        CheckUp(info);
    }
        
    info->nodes++;
    if(((IsRepetition(pos) && !listProvided) || pos->fiftyMove >= 100) && pos->ply) {    
        return 0;
    }
 
    if(pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }

    int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);
    
    if(InCheck == TRUE) {
        depth++;
    }
    
    int Score = -INFINITE;
    int PvMove = NOMOVE;

    if( ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE && !listProvided) {
        pos->HashTable->cut++;
        return Score;
    }

    if( DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 4) {
        MakeNullMove(pos);
    S_MOVELIST list2[1];    
        Score = -AlphaBeta( -beta, -beta + 1, depth-4, pos, info, FALSE, list2, FALSE);
        TakeNullMove(pos);
        if(info->stopped == TRUE) {
            return 0;
        }        
        
        if (Score >= beta && abs(Score) < ISMATE) {        
          return beta;
        }    
    }
   
    if(!listProvided)
        GenerateAllMoves(pos,list, FALSE);    
    int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;
    
    int BestScore = -INFINITE;
    
    Score = -INFINITE;
    
    if( PvMove != NOMOVE) {
        for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
            if( list->moves[MoveNum].move == PvMove) {
                list->moves[MoveNum].score = 2000000;
                break;
            }
        }
    }

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {    
            
        PickNextMove(MoveNum, list);    
        if (!MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        Legal++;
    S_MOVELIST list3[1];
        Score = -AlphaBeta( -beta, -alpha, depth-1, pos, info, TRUE, list3, FALSE);
        TakeMove(pos);
        
        if(info->stopped == TRUE) {
            return 0;
        }        
        if(Score > BestScore) {
            BestScore = Score;
            BestMove = list->moves[MoveNum].move;
            if(Score > alpha) {
                if(Score >= beta) {
                    if(Legal==1) {
                        info->fhf++;
                    }
                    info->fh++;    
                    
                    if(!(list->moves[MoveNum].move & MFLAGCAP)) {
                        pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
                        pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
                    }
                    
                    StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
                    
                    return beta;
                }
                alpha = Score;
                
                if(!(list->moves[MoveNum].move & MFLAGCAP)) {
                    pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
                }
            }
        }        
    }
    
    if(Legal == 0) {
        if(InCheck) {
            return -INFINITE + pos->ply;
        } else {
            return 0;
        }
    }
    
    if(alpha != OldAlpha) {
        StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
    } else {
        StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
    }
    
    return alpha;
    
}
static int CheckCaptures(S_BOARD *pos, int pvMove,S_MOVELIST *list, int bestScore)
{
    int counter;
    int score=-INFINITE;
    int index=0;

    int refutationMove=NOMOVE;
    int trapScore=-INFINITE;
    printf("pvmove is %s \n", PrMove(pvMove));
    MakeMove(pos, pvMove);
    GenerateAllMoves(pos, list, TRUE);

    for(counter=0;counter<list->count;counter++)
    {

        int capture=list->moves[counter].move;

        printf("capture is %s \n", PrMove(capture));

	int fromsq=FROMSQ(capture);
	int sq=TOSQ(capture);
	int side=pos->side;
	int oppositeSide=side ^ 1;
	int attackingPce=pos->pieces[fromsq];
	int attackedPce=pos->pieces[sq];

	if((EvalCapture(pos,oppositeSide, sq) < EvalCapture(pos,side, sq)) || (PieceVal[attackingPce]-10)<=PieceVal[attackedPce])
	{

		MakeMove(pos, capture);    
		S_MOVELIST moves[1];
		score=AlphaBeta(-INFINITE, INFINITE, TRAPSEARCHDEPTH, pos, info, TRUE, moves, FALSE);
		int index = pos->posKey % pos->HashTable->numEntries;
		if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
		    refutationMove=pos->HashTable->pTable[index].move;
		}

		printf("refutation is %s \n", PrMove(refutationMove));

		if(score-TRAPTHRESHOLD>bestScore)
		{
		    if (! (refutationMove & MFLAGCAP))
		    {
			printf("trap registered \n");
			if(score>trapScore)
			    trapScore=score;
		    }else{

			int capturedSq=TOSQ(refutationMove);
			if(capturedSq != sq)
			{
				printf("trap registered \n");
				if(score>trapScore)
					trapScore=score;
			}
		    }
		}
		TakeMove(pos);
	}
    }
    TakeMove(pos);
    return trapScore;


}
static void CheckUp(S_SEARCHINFO *info) {
    // .. check if time up, or interrupt from GUI
    if(info->timeset == TRUE && GetTimeMs() > info->stoptime) {
        info->stopped = TRUE;
    }
        
    ReadInput(info);
}
static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
    
    int index = 0;
    int index2 = 0;
    
    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < BRD_SQ_NUM; ++index2) {
            pos->searchHistory[index][index2] = 0;
        }
    }
    
    for(index = 0; index < 2; ++index) {
        for(index2 = 0; index2 < MAXDEPTH; ++index2) {
            pos->searchKillers[index][index2] = 0;
        }
    }    

    pos->HashTable->overWrite=0;
    pos->HashTable->hit=0;
    pos->HashTable->cut=0;    
    pos->ply = 0;
    
    info->stopped = 0;
    info->nodes = 0;
    info->fh = 0;
    info->fhf = 0;
}
static void ClearPiece(const int sq, S_BOARD *pos) {

    
    int pce = pos->pieces[sq];
    
    
    int col = PieceCol[pce];
    int index = 0;
    int t_pceNum = -1;
    
    HASH_PCE(pce,sq);
    
    pos->pieces[sq] = EMPTY;
    pos->material[col] -= PieceVal[pce];
    
    if(PieceBig[pce]) {
        pos->bigPce[col]--;
        
    } else {
        CLRBIT(pos->pawns[col],SQ64(sq));
        CLRBIT(pos->pawns[BOTH],SQ64(sq));
    }
    
    for(index = 0; index < pos->pceNum[pce]; ++index) {
        if(pos->pList[pce][index] == sq) {
            t_pceNum = index;
            break;
        }
    }
    
    
    pos->pceNum[pce]--;        
    
    pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];
 
}
static void ClearHashTable(S_HASHTABLE *table) {
    S_HASHENTRY *tableEntry;
 
    for (tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++)
    {
        tableEntry->posKey = 0ULL;
        tableEntry->move = NOMOVE;
        tableEntry->depth = 0;
        tableEntry->score = 0;
        tableEntry->flags = 0;
     }
     table->newWrite=0;
 }
static void Console_Loop()
{
    
    printf("mode is console\n");
    S_MOVELIST list[1];
    while(1)
    {
        memset(line, 0, sizeof(line));

        fgets(line, INPUTBUFFER, stdin);
        if(strncmp(line, "fen", 3)==0)
        {
            line += 4;
            ParseFen(line, pos);    
        }else if (strncmp(line, "go", 2)==0)
        {
            ParseGo(line, info, pos);
        }else if (strncmp(line, "eval", 4)==0)
        {
            PrintPositionalEvals(pos);        
        }else if (strncmp(line, "genmoves", 8)==0)
    {
        GenerateAllMoves(pos, list, FALSE);
        PrintMoves(list);
    
    }else if (strncmp (line, "printmoves", 10)==0)
    {
        PrintMoves(list);
    }
    else if(strncmp(line,"popmove", 7)==0)
    {
        ParsePop(line, list);
    }
    else if(strncmp(line, "q", 1)==0)
        {
            break;        
        }
    }
}
int CountBits(U64 b) {
  int r;
  for(r = 0; b; r++, b &= b - 1);
  return r;
}
static int EvalCapture(const S_BOARD *pos, const int side, const int sq)

{
    int pce,index,t_sq,dir;
    int count=0;
    
    // pawn
    if(side == WHITE) {
        if(pos->pieces[sq-11] == wP) {
            count ++;
        }
	if(pos->pieces[sq-9]==wP){
	    count ++;
	}
    } else {
        if(pos->pieces[sq+11] == bP) {
            count ++;
        }   
	if(pos->pieces[sq+9] == bP) {
            count ++;
        }    
    }
    
    // knights
    for(index = 0; index < 8; ++index) {        
        pce = pos->pieces[sq + KnDir[index]];
        if(pce != OFFBOARD && IsKn(pce) && PieceCol[pce]==side) {
            count ++;

        }
    }
    
    // rooks, queens
    for(index = 0; index < 4; ++index) {        
        dir = RkDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsRQ(pce) && PieceCol[pce] == side) {

                    count ++;
                }else
			break;
                
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }
    
    // bishops, queens
    for(index = 0; index < 4; ++index) {        
        dir = BiDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsBQ(pce) && PieceCol[pce] == side) {
                    count ++;

                }else
			break;
                
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }
    
    // kings
    for(index = 0; index < 8; ++index) {        
        pce = pos->pieces[sq + KiDir[index]];
        if(pce != OFFBOARD && IsKi(pce) && PieceCol[pce]==side) {
            count ++;

        }
    }
    
    return count;

}
static int EvalPosition(const S_BOARD *pos) {

    int pce;
    int pceNum;
    int sq;
    int score = pos->material[WHITE] - pos->material[BLACK];
    if(!pos->pceNum[wP] && !pos->pceNum[bP]) {
        if(MaterialDraw(pos))
            return 0;
    }
    if(pos->material[BLACK]>0)
    {
        
        if (!pos->isCastled[WHITE])
        {
            score -= 20;
        }

        score += EvalWhitePawns(pos);
        score += EvalWhiteKnight(pos);
        score += EvalWhiteBishop(pos);
        score += EvalWhiteRook(pos);
        score += EvalWhiteQueen(pos);

    }
    score += EvalWhiteKing(pos);
    if(pos->material[WHITE]>0)
    {
        if (!pos->isCastled[BLACK])
        {
            score += 20;
        }
        

        score += EvalBlackPawns(pos);
        score += EvalBlackKnight(pos);
        score += EvalBlackBishop(pos);
        score += EvalBlackRook(pos);
        score += EvalBlackQueen(pos);

    }
    
    score += EvalBlackKing(pos);

    if(pos->side == WHITE) {
        return score;
    } else {
        return -score;
    }        
}
static int FileRankValid(const int fr) {
    return (fr >= 0 && fr <= 7) ? 1 : 0;
}
int FindBit(U64 bb) {
  U64 b = bb ^ (bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  return BitTable[(fold * 0x783a9b23) >> 26];
}
static
void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list, int cap_only) {
    
    
    list->count = 0;    
    
    int pce = EMPTY;
    int side = pos->side;
    int sq = 0; int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;
    
    if(side == WHITE) {
        
        for(pceNum = 0; pceNum < pos->pceNum[wP]; ++pceNum) {
            sq = pos->pList[wP][pceNum];
            
            if(pos->pieces[sq + 10] == EMPTY && !cap_only) {
                AddPawnMove(pos, sq, sq+10, list);
                if(RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            
            if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK) {
                AddPawnCapMove(pos, sq, sq+9, pos->pieces[sq + 9], list);
            }  
            if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK) {
                AddPawnCapMove(pos, sq, sq+11, pos->pieces[sq + 11], list);
            }
            
            if(pos->enPas != NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq + 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq + 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }
        
        if(pos->castlePerm & WKCA && !cap_only) {
            if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
                if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(F1,BLACK,pos) ) {
                    AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        
        if(pos->castlePerm & WQCA && !cap_only) {
            if(pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1] == EMPTY) {
                if(!SqAttacked(E1,BLACK,pos) && !SqAttacked(D1,BLACK,pos) ) {
                    AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        
    } else {
        
        for(pceNum = 0; pceNum < pos->pceNum[bP]; ++pceNum) {
            sq = pos->pList[bP][pceNum];
            
            if(pos->pieces[sq - 10] == EMPTY && !cap_only) {
                AddPawnMove(pos, sq, sq-10, list);
                if(RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY) {
                    AddQuietMove(pos, MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS),list);
                }
            }
            
            if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE) {
                AddPawnCapMove(pos, sq, sq-9, pos->pieces[sq - 9], list);
            }
            
            if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE) {
                AddPawnCapMove(pos, sq, sq-11, pos->pieces[sq - 11], list);
            }
            if(pos->enPas != NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 9,EMPTY,EMPTY,MFLAGEP), list);
                }
                if(sq - 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq,sq - 11,EMPTY,EMPTY,MFLAGEP), list);
                }
            }
        }        
        
        // castling
        if(pos->castlePerm &  BKCA && !cap_only) {
            if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
                if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(F8,WHITE,pos) ) {
                    AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        
        if(pos->castlePerm &  BQCA && !cap_only) {
            if(pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8] == EMPTY) {
                if(!SqAttacked(E8,WHITE,pos) && !SqAttacked(D8,WHITE,pos) ) {
                    AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }
    
    /* Loop for slide pieces */
    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePce[pceIndex++];
    while( pce != 0) {
        
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            
            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;
                
                while(!SQOFFBOARD(t_sq)) {                
                    // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                    if(pos->pieces[t_sq] != EMPTY) {
                        if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    if (!cap_only)    
                        AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }
        
        pce = LoopSlidePce[pceIndex++];
    }

    /* Loop for non slide */
    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePce[pceIndex++];
    
    while( pce != 0) {
        
        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            
            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;
                
                if(SQOFFBOARD(t_sq)) {                    
                    continue;
                }
                
                // BLACK ^ 1 == WHITE       WHITE ^ 1 == BLACK
                if(pos->pieces[t_sq] != EMPTY) {
                    if( PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }    
                if (!cap_only)
                    AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
            }
        }
                
        pce = LoopNonSlidePce[pceIndex++];
    }
}



static U64 GeneratePosKey(const S_BOARD *pos) {

    int sq = 0;
    U64 finalKey = 0;
    int piece = EMPTY;
    
    // pieces
    for(sq = 0; sq < BRD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if(piece!=NO_SQ && piece!=EMPTY && piece != OFFBOARD) {
            finalKey ^= PieceKeys[piece][sq];
        }        
    }
    
    if(pos->side == WHITE) {
        finalKey ^= SideKey;
    }
        
    if(pos->enPas != NO_SQ) {
        finalKey ^= PieceKeys[EMPTY][pos->enPas];
    }
    
    finalKey ^= CastleKeys[pos->castlePerm];
    
    return finalKey;
}
static
int GetPvLine(const int depth, S_BOARD *pos) {


    int move = ProbePvTable(pos);
    int count = 0;
    
    while(move != NOMOVE && count < depth) {
    
    
        if( MoveExists(pos, move) ) {
            MakeMove(pos, move);
            pos->PvArray[count++] = move;
        } else {
            break;
        }        
        move = ProbePvTable(pos);    
    }
    
    while(pos->ply > 0) {
        TakeMove(pos);
    }
    
    return count;
    
}
static
int GetTimeMs() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;
}

static
void Init() {
    InitSq120To64();    
    InitBitMasks();
    InitHashKeys();
    InitFilesRanksBrd();
    InitMvvLva();
    InitEvalMasks();
}

static
void InitBitMasks() {
    int index = 0;
    
    for(index = 0; index < 64; index++) {
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }
    
    for(index = 0; index < 64; index++) {
        SetMask[index] |= (1ULL << index);
        ClearMask[index] = ~SetMask[index];
    }
}
static
void InitEvalMasks() {

    int sq, tsq, r, f, index;
//File and rank masks        
    for(sq = 0; sq < 8; ++sq) {        
        FileBBMask[sq] = 0ULL;
        RankBBMask[sq] = 0ULL;
    }
    
    for(r = RANK_8; r >= RANK_1; r--) {
        for (f = FILE_A; f <= FILE_H; f++) {
            sq = r * 8 + f;
            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }
    }
    //passed and isolated masks
    for(sq = 0; sq < 64; ++sq) {
        IsolatedMask[sq] = 0ULL;
        WhitePassedMask[sq] = 0ULL;
        BlackPassedMask[sq] = 0ULL;
        }

    for(sq = 0; sq < 64; ++sq) {

        
        tsq = sq + 8;
            
        while(tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if(FilesBrd[SQ120(sq)] > FILE_A) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
            }
        }
            
        if(FilesBrd[SQ120(sq)] < FILE_H) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[SQ120(sq)] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
            }
        }
    }
//color square
    int color=BLACK;
    for(sq=0;sq<64;sq++)
    {
        //color doesnt change with a new rank
        if(sq % 8 !=0)
        {
        //BLACK ^ 1=WHITE WHITE ^ 1=BLACK
            color^=1;
        }
        ColorSquare[sq]=color;
    }

//king manhattan distances
    for(sq=0;sq<64;sq++)
    {
        for(tsq=0;tsq<64;tsq++)
        {
            int sqFile=FilesBrd[SQ120(sq)];
            int sqRank=RanksBrd[SQ120(sq)];
            int tsqFile=FilesBrd[SQ120(tsq)];
            int tsqRank=RanksBrd[SQ120(tsq)];
            int fileDistance=abs(sqFile-tsqFile);
            int rankDistance=abs(sqRank-tsqRank);
            KingManhattanDistances[sq][tsq]=fileDistance+rankDistance;
        //    printf("sq is %d tsq is %d distance is %d \n", sq,tsq,fileDistance+rankDistance);
        }
        
    
    }
//doubled pawns masks
    for(sq=0;sq<64;sq++)
    {
        tsq=SQ120(sq)+10;
        while(SqOnBoard(tsq))
        {
            DoubledPawnsMask[sq] |= (1ULL<<SQ64(tsq));
            tsq += 10;
        }
    }
// max Diagonal Distances
    for(sq=0;sq<64;sq++)
    {
        tsq=SQ120(sq)+11;
        while(SqOnBoard(tsq))
        {
            WhiteMaxDiagonalDistance[sq][0] ++;
            tsq += 11;
        }
        tsq=SQ120(sq)+9;
        while(SqOnBoard(tsq))
        {
            WhiteMaxDiagonalDistance[sq][1] ++;
            tsq += 9;
        }
        tsq=SQ120(sq)-11;
        while(SqOnBoard(tsq))
        {
            BlackMaxDiagonalDistance[sq][0] ++;
            tsq -= 11;
        }
        tsq=SQ120(sq)-9;
        while(SqOnBoard(tsq))
        {
            BlackMaxDiagonalDistance[sq][1] ++;
            tsq -= 9;
        }

    }
//diagonal distance
    for(sq=0;sq<64;sq++)
    {
        for(tsq=0;tsq<64;tsq++)
        {
            if((tsq-sq)%9==0)
                DiagonalDistance[sq][tsq]=(tsq-sq)/9;
            else if((tsq-sq)%7==0)
                DiagonalDistance[sq][tsq]=(tsq-sq)/7;

        }    
    }

//outpost masks
    for(sq=0;sq<64;sq++)
    {
        int file=FilesBrd[SQ120(sq)];
        WhiteOutpostMask[sq] = WhitePassedMask[sq] ^ (FileBBMask[file] & WhitePassedMask[sq]);
        BlackOutpostMask[sq] = BlackPassedMask[sq] ^ (FileBBMask[file] & BlackPassedMask[sq]);
    }
//knight mobility mask
    for(sq=0;sq<64;sq++)
    {
        for(index=0;index<8;index++)
        {
            tsq=SQ120(sq)+KnDir[index];
            if(SqOnBoard(tsq))
                KnightMobilityMask[sq] |= (1ULL<<(SQ64(tsq)));
            if(SqOnBoard(tsq-11))
                BlackKnightMobilityMask[sq] |= (1ULL<<(SQ64(tsq)-9));
            if(SqOnBoard(tsq-9))
                BlackKnightMobilityMask[sq] |= (1ULL<<(SQ64(tsq)-7));
            if(SqOnBoard(tsq+11))
                WhiteKnightMobilityMask[sq] |= (1ULL<<(SQ64(tsq)+9));
            if(SqOnBoard(tsq+9))
                WhiteKnightMobilityMask[sq] |= (1ULL<<(SQ64(tsq)+7));
        }
        KnightMobility[sq]=CountBits(KnightMobilityMask[sq]);
    }
//pawn support masks
    for(sq=0;sq<64;sq++)
    {
        if(SqOnBoard(SQ120(sq)-11))
            WhitePawnSupportMask[sq] |= (1ULL<<(sq-9));
        if(SqOnBoard(SQ120(sq)-9))
            WhitePawnSupportMask[sq] |= (1ULL<<(sq-7));
        if(SqOnBoard(SQ120(sq)+11))
            BlackPawnSupportMask[sq] |= (1ULL<<(sq+9));
        if(SqOnBoard(SQ120(sq)+9))
            BlackPawnSupportMask[sq] |= (1ULL<<(sq+7));

    }
//bishop forward masks
    for (sq=0;sq<64;sq++)
    {
        //white forwards
        tsq=SQ120(sq)+11;
        while(SqOnBoard(tsq))
        {
            WhiteBishopForwardMask[sq][0] |= (1ULL<<(SQ64(tsq)));
            tsq += 11;
        }
        tsq=SQ120(sq)+9;
        while(SqOnBoard(tsq))
        {
            WhiteBishopForwardMask[sq][1] |= (1ULL<<(SQ64(tsq)));
            tsq += 9;
        }
        //black forwards
        tsq=SQ120(sq)-11;
        while(SqOnBoard(tsq))
        {
            BlackBishopForwardMask[sq][0] |= (1ULL<<(SQ64(tsq)));
            tsq -= 11;
        }
        tsq=SQ120(sq)-9;
        while(SqOnBoard(tsq))
        {
            BlackBishopForwardMask[sq][1] |= (1ULL<<(SQ64(tsq)));
            tsq -= 9;
        }

    }

}
static
void InitFilesRanksBrd() {
    
    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;
    
    for(index = 0; index < BRD_SQ_NUM; ++index){
        FilesBrd[index] = OFFBOARD;
        RanksBrd[index] = OFFBOARD;
    }
    
    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file,rank);
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}
static
void InitHashKeys() {
    
    int index = 0;
    int index2 = 0;
    for(index = 0; index < 13; ++index) {
        for(index2 = 0; index2 < 120; ++index2) {
            PieceKeys[index][index2] = RAND_64;
        }
    }
    SideKey = RAND_64;
    for(index = 0; index < 16; ++index) {
        CastleKeys[index] = RAND_64;
    }

}
static
int InitMvvLva() {
    int Attacker;
    int Victim;
    for(Attacker = wP; Attacker <= bK; ++Attacker) {
        for(Victim = wP; Victim <= bK; ++Victim) {
            MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - ( VictimScore[Attacker] / 100);
        }
    }        
}
static
void InitHashTable(S_HASHTABLE *table) {  
 
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;
    if(table->pTable != NULL) free(table->pTable);
    table->pTable = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
    ClearHashTable(table);
    
}
static void InitSq120To64() {

    int index = 0;
    int file = FILE_A;
    int rank = RANK_1;
    int sq = A1;
    int sq64 = 0;
    for(index = 0; index < BRD_SQ_NUM; ++index) {
        Sq120ToSq64[index] = 65;
    }
    
    for(index = 0; index < 64; ++index) {
        Sq64ToSq120[index] = 120;
    }
    
    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file,rank);
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}
static
int InputWaiting()
{
  fd_set readfds;
  struct timeval tv;
  FD_ZERO (&readfds);
  FD_SET (fileno(stdin), &readfds);
  tv.tv_sec=0; tv.tv_usec=0;
  select(16, &readfds, 0, 0, &tv);

  return (FD_ISSET(fileno(stdin), &readfds));
}
static
int IsRepetition(const S_BOARD *pos) {

    int index = 0;

    for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly-1; ++index) {    
        if(pos->posKey == pos->history[index].posKey) {
            return TRUE;
        }
    }    
    return FALSE;
}
static
int MakeMove(S_BOARD *pos, int move) {

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;
    
    pos->history[pos->hisPly].posKey = pos->posKey;
    
    if(move & MFLAGEP) {
        if(side) {
            ClearPiece(to+10,pos);
        } else {
            ClearPiece(to-10,pos);
        }
    } else if (move & MFLAGCA) {
        switch(to) {
            case C1:
        pos->isCastled[WHITE]=TRUE;
                MovePiece(A1, D1, pos);
            break;
            case C8:
        pos->isCastled[BLACK]=TRUE;
                MovePiece(A8, D8, pos);
            break;
            case G1:
        pos->isCastled[WHITE]=TRUE;
                MovePiece(H1, F1, pos);
            break;
            case G8:
        pos->isCastled[BLACK]=TRUE;
                MovePiece(H8, F8, pos);
            break;
            default: break;
        }
    }    
    
    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;
    
    pos->history[pos->hisPly].move = move;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;

    pos->castlePerm &= CastlePerm[from];
    pos->castlePerm &= CastlePerm[to];
    pos->enPas = NO_SQ;

    HASH_CA;
    
    int captured = CAPTURED(move);
    pos->fiftyMove++;
    
    if(captured != EMPTY) {
        ClearPiece(to, pos);
        pos->fiftyMove = 0;
    }
    
    pos->hisPly++;
    pos->ply++;
    
    if(PiecePawn[pos->pieces[from]]) {
        pos->fiftyMove = 0;
        if(move & MFLAGPS) {
            if(side==WHITE) {
                pos->enPas=from+10;
            } else {
                pos->enPas=from-10;
            }
            HASH_EP;
        }
    }
    
    MovePiece(from, to, pos);
    
    int prPce = PROMOTED(move);
    if(prPce != EMPTY)   {
        ClearPiece(to, pos);
        AddPiece(to, pos, prPce);
    }
    
    if(PieceKing[pos->pieces[to]]) {
        pos->KingSq[pos->side] = to;
    }
    
    pos->side ^= 1;
    HASH_SIDE;

    
        
    if(SqAttacked(pos->KingSq[side],pos->side,pos))  {
        TakeMove(pos);
        return FALSE;
    }
    
    return TRUE;
}
static    
void MakeNullMove(S_BOARD *pos) {

    pos->ply++;
    pos->history[pos->hisPly].posKey = pos->posKey;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->history[pos->hisPly].move = NOMOVE;
    pos->history[pos->hisPly].fiftyMove = pos->fiftyMove;
    pos->history[pos->hisPly].enPas = pos->enPas;
    pos->history[pos->hisPly].castlePerm = pos->castlePerm;
    pos->enPas = NO_SQ;

    pos->side ^= 1;
    pos->hisPly++;
    HASH_SIDE;
   
    return;
}
int MaterialDraw(const S_BOARD *pos) {
        if (!pos->pceNum[wR] && !pos->pceNum[bR] && !pos->pceNum[wQ] && !pos->pceNum[bQ]) {
      if (!pos->pceNum[bB] && !pos->pceNum[wB]) {
          if (pos->pceNum[wN] < 3 && pos->pceNum[bN] < 3) {  return TRUE; }
      } else if (!pos->pceNum[wN] && !pos->pceNum[bN]) {
         if (abs(pos->pceNum[wB] - pos->pceNum[bB]) < 2) { return TRUE; }
      } else if ((pos->pceNum[wN] < 3 && !pos->pceNum[wB]) || (pos->pceNum[wB] == 1 && !pos->pceNum[wN])) {
        if ((pos->pceNum[bN] < 3 && !pos->pceNum[bB]) || (pos->pceNum[bB] == 1 && !pos->pceNum[bN]))  { return TRUE; }
      }
    }
    return FALSE;
}
static
int MoveExists(S_BOARD *pos, const int move) {
    
    S_MOVELIST list[1];
        GenerateAllMoves(pos,list,0);
      
        int MoveNum = 0;
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {    
       
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }        
        TakeMove(pos);
        if(list->moves[MoveNum].move == move) {
            return TRUE;
        }
    }
    return FALSE;
}
static
void MovePiece(const int from, const int to, S_BOARD *pos) {

    int index = 0;
    int pce = pos->pieces[from];    
    int col = PieceCol[pce];
    
    int t_PieceNum = FALSE;

    HASH_PCE(pce,from);
    pos->pieces[from] = EMPTY;
    
    HASH_PCE(pce,to);
    pos->pieces[to] = pce;
    
    if(!PieceBig[pce]) {
        CLRBIT(pos->pawns[col],SQ64(from));
        CLRBIT(pos->pawns[BOTH],SQ64(from));
        SETBIT(pos->pawns[col],SQ64(to));
        SETBIT(pos->pawns[BOTH],SQ64(to));        
    }    
    
    for(index = 0; index < pos->pceNum[pce]; ++index) {
        if(pos->pList[pce][index] == from) {
            pos->pList[pce][index] = to;
            t_PieceNum = TRUE;
            break;
        }
    }
}
static
int ParseFen(char *fen, S_BOARD *pos)
{
       int  rank = RANK_8;
        int  file = FILE_A;
        int  piece = 0;
        int  count = 0;
        int  i = 0;
    int  sq64 = 0;
    int  sq120 = 0;
    
    ResetBoard(pos);
    
    while ((rank >= RANK_1) && *fen) {
        count = 1;
        switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count = *fen - '0';
                break;

            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;              

            default:
                return -1;
        }        
        
        for (i = 0; i < count; i++) {            
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        fen++;
    }
    
    
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;
    
    for (i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }        
        switch(*fen) {
            case 'K': pos->castlePerm |= WKCA; break;
            case 'Q': pos->castlePerm |= WQCA; break;
            case 'k': pos->castlePerm |= BKCA; break;
            case 'q': pos->castlePerm |= BQCA; break;
            default:         break;
        }
        fen++;
    }
    fen++;
    
    if (*fen != '-') {        
        file = fen[0] - 'a';
        rank = fen[1] - '1';
        
        pos->enPas = FR2SQ(file,rank);        
    }
    
    pos->posKey = GeneratePosKey(pos);
    
    UpdateListsMaterial(pos);
    
    return 0;
}
static
void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {
    
    int depth = -1, movestogo = 30,movetime = -1;
    int time = -1, inc = 0;
        char *ptr = NULL;
    info->timeset = FALSE;
    
    if ((ptr = strstr(line,"infinite"))) {
        ;
    }
    
    if ((ptr = strstr(line,"binc")) && pos->side == BLACK) {
        inc = atoi(ptr + 5);
    }
    
    if ((ptr = strstr(line,"winc")) && pos->side == WHITE) {
        inc = atoi(ptr + 5);
    }
    
    if ((ptr = strstr(line,"wtime")) && pos->side == WHITE) {
        time = atoi(ptr + 6);
    }
      
    if ((ptr = strstr(line,"btime")) && pos->side == BLACK) {
        time = atoi(ptr + 6);
    }
      
    if ((ptr = strstr(line,"movestogo"))) {
        movestogo = atoi(ptr + 10);
    }
      
    if ((ptr = strstr(line,"movetime"))) {
        movetime = atoi(ptr + 9);
    }
      
    if ((ptr = strstr(line,"depth"))) {
        depth = atoi(ptr + 6);
    }
    
    if(movetime != -1) {
        time = movetime;
        movestogo = 1;
    }
    
    info->starttime = GetTimeMs();
    info->depth = depth;
    
    if(time != -1) {
        info->timeset = TRUE;
        time /= movestogo;
        time -= 50;        
        info->stoptime = info->starttime + time + inc;
    }
    
    if(depth == -1) {
        info->depth = MAXDEPTH;
    }
    
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
        time,info->starttime,info->stoptime,info->depth,info->timeset);
    SearchPosition(pos, info);
}
static void ParsePop(char *lineIn, S_MOVELIST *list)
{
    
    lineIn += 8;
    char *ptrChar=lineIn;

    int move=atoi(ptrChar);
    printf("popping move %d \n", move);
    PopMove(list, move);

}
static
void ParsePosition(char* lineIn, S_BOARD *pos) {
    
    lineIn += 9;
    char *ptrChar = lineIn;
    
    if(strncmp(lineIn, "startpos", 8) == 0){
        ParseFen(START_FEN, pos);
    } else {
        ptrChar = strstr(lineIn, "fen");
        if(ptrChar == NULL) {
            ParseFen(START_FEN, pos);
        } else {
            ptrChar+=4;
            ParseFen(ptrChar, pos);
        }
    }
    
    ptrChar = strstr(lineIn, "moves");
    int move;
    
    if(ptrChar != NULL) {
        ptrChar += 6;
        while(*ptrChar) {
              move = ParseMove(ptrChar,pos);
              if(move == NOMOVE) break;
              MakeMove(pos, move);
              pos->ply=0;
              while(*ptrChar && *ptrChar!= ' ') ptrChar++;
              ptrChar++;
        }
    }
}
static
int ParseMove(char *ptrChar, S_BOARD *pos) {
    if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');    
    
    
    S_MOVELIST list[1];
    GenerateAllMoves(pos,list,0);      
    int MoveNum = 0;
    int Move = 0;
    int PromPce = EMPTY;
    
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {    
        Move = list->moves[MoveNum].move;
        if(FROMSQ(Move)==from && TOSQ(Move)==to) {
            PromPce = PROMOTED(Move);
            if(PromPce!=EMPTY) {
                if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4]=='r') {
                    return Move;
                } else if(!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='b') {
                    return Move;
                } else if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4]=='q') {
                    return Move;
                } else if(IsKn(PromPce)&& ptrChar[4]=='n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }
printf("returned a move\n");    
    return NOMOVE;    
}static
void PickNextMove(int moveNum, S_MOVELIST *list) {

    S_MOVE temp;
    int index = 0;
    int bestScore = 0;
    int bestNum = moveNum;
    
    for (index = moveNum; index < list->count; ++index) {
        if (list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }
    temp = list->moves[moveNum];
    list->moves[moveNum] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}static
int PieceValid(const int pce) {
    return (pce >= wP && pce <= bK) ? 1 : 0;
}static
int PieceValidEmpty(const int pce) {
    return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}
static void PopMove(S_MOVELIST *list, int move)
{
    int counter;
    int index=-1;
    for(counter=0;counter<list->count; counter++)
    {
        if(move==list->moves[counter].move)
            index=counter;
    }
    if(index==-1)
        return;
    for(counter=index; counter<list->count-1; counter++)
    {
        list->moves[counter]=list->moves[counter+1];

    }
    list->count --;
    

}

void PrintBitBoard(U64 bb) {

    U64 shiftMe = 1ULL;
    
    int rank = 0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;
    
    printf("\n");
    for(rank = RANK_1; rank <= RANK_8; ++rank) {
        for(file = FILE_A; file <= FILE_H; ++file) {
            sq = FR2SQ(file,rank);    // 120 based        
            sq64 = SQ64(sq); // 64 based
            
            if((shiftMe << sq64) & bb)
                printf("X");
            else
                printf("-");
                
        }
        printf("\n");
    }  
    printf("\n\n");
}
void PrintBoard(const S_BOARD *pos) {
    
    int sq,file,rank,piece;

    printf("\nGame Board:\n\n");
    
    for(rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d  ",rank+1);
        for(file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            piece = pos->pieces[sq];
            printf("%3c",PceChar[piece]);
        }
        printf("\n");
    }
    
    printf("\n   ");
    for(file = FILE_A; file <= FILE_H; file++) {
        printf("%3c",'a'+file);    
    }
    printf("\n");
    printf("side:%c\n",SideChar[pos->side]);
    printf("enPas:%d\n",pos->enPas);
    printf("castle:%c%c%c%c\n",
            pos->castlePerm & WKCA ? 'K' : '-',
            pos->castlePerm & WQCA ? 'Q' : '-',
            pos->castlePerm & BKCA ? 'k' : '-',
            pos->castlePerm & BQCA ? 'q' : '-'    
            );
    printf("PosKey:%llX\n",pos->posKey);
}
static void PrintMoves(const S_MOVELIST *list)
{
    for(int counter=0;counter<list->count;counter++)
    {
        printf("move %d %s\n", list->moves[counter].move, PrMove(list->moves[counter].move));

    }

}
static
void PrintPositionalEvals(S_BOARD *pos)
{    
    
    int score=0;

    score=EvalWhitePawns(pos);
    printf("White pawn score is %d \n", score);

    score=EvalBlackPawns(pos);        
    printf("Black pawn score is %d \n", score);

    score=EvalWhiteKnight(pos);
    printf("White Knight score is %d \n", score);

    score=EvalBlackKnight(pos);        
    printf("Black knight score is %d \n", score);

    score=EvalWhiteBishop(pos);
    printf("white bishop score is %d \n", score);

    score=EvalBlackBishop(pos);
    printf("Black bishop score is %d \n", score);

    score=EvalWhiteRook(pos);
    printf("white rook score is %d \n", score);

    score=EvalBlackRook(pos);
    printf("Black rook score is %d \n", score);

    score=EvalWhiteQueen(pos);
    printf("white queen score is %d \n", score);

    score=EvalBlackQueen(pos);
    printf("Black queen score is %d \n", score);

    score=EvalWhiteKing(pos);
    printf("white king score is %d \n", score);

    score=EvalBlackKing(pos);
    printf("Black king score is %d \n", score);




}
static
char *PrMove(const int move) {

    static char MvStr[6];
    
    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];
    
    int promoted = PROMOTED(move);
    
    if(promoted) {
        char pchar = 'q';
        if(IsKn(promoted)) {
            pchar = 'n';
        } else if(IsRQ(promoted) && !IsBQ(promoted))  {
            pchar = 'r';
        } else if(!IsRQ(promoted) && IsBQ(promoted))  {
            pchar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else {
        sprintf(MvStr, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
    }
    return MvStr;
}
static
int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {

    int index = pos->posKey % pos->HashTable->numEntries;
    
    
    if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
        *move = pos->HashTable->pTable[index].move;
        if(pos->HashTable->pTable[index].depth >= depth){
            pos->HashTable->hit++;
            
            *score = pos->HashTable->pTable[index].score;
            if(*score > ISMATE) *score -= pos->ply;
            else if(*score < -ISMATE) *score += pos->ply;
            
            switch(pos->HashTable->pTable[index].flags) {
                

                case HFALPHA: if(*score<=alpha) {
                    *score=alpha;
                    return TRUE;
                    }
                    break;
                case HFBETA: if(*score>=beta) {
                    *score=beta;
                    return TRUE;
                    }
                    break;
                case HFEXACT:
                    return TRUE;
                    break;
                default: break;
            }
        }
    }
    
    return FALSE;
}

static
int ProbePvTable(const S_BOARD *pos) {

    int index = pos->posKey % pos->HashTable->numEntries;
    
    if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
        return pos->HashTable->pTable[index].move;
    }
    
    return NOMOVE;
}
static
int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
if(( info->nodes & 2047 ) == 0) {
        CheckUp(info);
    }
    
    info->nodes++;
    
    if(IsRepetition(pos) || pos->fiftyMove >= 100) {
        return 0;
    }
    
    if(pos->ply > MAXDEPTH - 1) {
        return EvalPosition(pos);
    }
    
    int Score = EvalPosition(pos);
    
    if(Score >= beta) {
        return beta;
    }
    
    if(Score > alpha) {
        alpha = Score;
    }
    
    S_MOVELIST list[1];
        GenerateAllMoves(pos,list,TRUE);
      
        int MoveNum = 0;
    int Legal = 0;
    int OldAlpha = alpha;
    int BestMove = NOMOVE;
    Score = -INFINITE;
    
    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {    
            
        PickNextMove(MoveNum, list);    
        
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
        Legal++;
        Score = -Quiescence( -beta, -alpha, pos, info);        
        TakeMove(pos);
        
        if(info->stopped == TRUE) {
            return 0;
        }
        
        if(Score > alpha) {
            if(Score >= beta) {
                if(Legal==1) {
                    info->fhf++;
                }
                info->fh++;    
                return beta;
            }
            alpha = Score;
            BestMove = list->moves[MoveNum].move;
        }        
    }
    
    return alpha;
}
static
void ReadInput(S_SEARCHINFO *info) {
    int bytes;
      char input[256] = "", *endc;

      if (InputWaiting()) {    
        info->stopped = TRUE;
        return;
        }
}
static
void ResetBoard(S_BOARD *pos) {

    int index = 0;
    
    for(index = 0; index < BRD_SQ_NUM; ++index) {
        pos->pieces[index] = OFFBOARD;
    }
    
    for(index = 0; index < 64; ++index) {
        pos->pieces[SQ120(index)] = EMPTY;
    }
    
    for(index = 0; index < 2; ++index) {
        pos->bigPce[index] = 0;
        pos->material[index] = 0;        
    }
    
    for(index = 0; index < 3; ++index) {
        pos->pawns[index] = 0ULL;
    }
    
    for(index = 0; index < 13; ++index) {
        pos->pceNum[index] = 0;
    }
    
    pos->KingSq[WHITE] = pos->KingSq[BLACK] = NO_SQ;
    
    pos->side = BOTH;
    pos->enPas = NO_SQ;
    pos->fiftyMove = 0;
    
    pos->ply = 0;
    pos->hisPly = 0;
    
    pos->castlePerm = 0;
    
    pos->posKey = 0ULL;
    
}
static int Search(int trap)
{
    int bestMove = NOMOVE;
    int bestScore = -INFINITE;
    int currentDepth = 0;
    int pvMoves = 0;
    int pvNum = 0;
    
    ClearForSearch(pos,info);
    
    // iterative deepening
    for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
    S_MOVELIST list[1];
                            // alpha     beta
        bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info,TRUE, list, FALSE);
        
        if(info->stopped == TRUE) {
            break;
        }
        
        pvMoves = GetPvLine(currentDepth, pos);
        bestMove = pos->PvArray[0];
        
        printf("info score cp %d depth %d nodes %ld time %d ",
            bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
            
        pvMoves = GetPvLine(currentDepth, pos);    
        printf("pv");        
        for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
            printf(" %s",PrMove(pos->PvArray[pvNum]));
        }
        printf("\n");
        printf("Hits: %d Overwrite:%d NewWrite:%d Cut:%d\n",pos->HashTable->hit, pos->HashTable->overWrite,pos->HashTable->newWrite, pos->HashTable->cut);
        // printf("Ordering:%.2f\n",(info->fhf/info->fh));
    }
    return bestMove;
}
static
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {

    int lastMove = pos->history[pos->hisPly-1].move;
    printf("lastMove is %s\n", PrMove(lastMove)) ;
    printf("expected move is %s \n", PrMove(ExpectedMove));
    if((trapMove != NOMOVE) && (lastMove == ExpectedMove))
    {
    printf("playing trap \n");
    printf("bestmove %s \n", PrMove(trapMove));
    trapMove=NOMOVE;
    ExpectedMove=NOMOVE;
    return;
    
    }
    int move=Search(FALSE);
    printf("bestmove %s\n",PrMove(move));    

    ClearForSearch(pos, info);
    info->starttime = GetTimeMs();
    info->depth = MAXDEPTH;
    info->timeset=FALSE;

    MakeMove(pos, move);
    int expectedMove=ProbePvTable(pos);
    if(expectedMove==NOMOVE)
    return;
    ExpectedMove=expectedMove;
    printf("expecting move %s from oponent \n",PrMove(expectedMove));    
    
    MakeMove(pos, expectedMove);
    TrapSearch(pos, info);
}
int SqAttacked(const int sq, const int side, const S_BOARD *pos) {

    int pce,index,t_sq,dir;
    
    // pawn
    if(side == WHITE) {
        if(pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
            return TRUE;
        }
    } else {
        if(pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
            return TRUE;
        }    
    }
    
    // knights
    for(index = 0; index < 8; ++index) {        
        pce = pos->pieces[sq + KnDir[index]];
        if(pce != OFFBOARD && IsKn(pce) && PieceCol[pce]==side) {
            return TRUE;
        }
    }
    
    // rooks, queens
    for(index = 0; index < 4; ++index) {        
        dir = RkDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsRQ(pce) && PieceCol[pce] == side) {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }
    
    // bishops, queens
    for(index = 0; index < 4; ++index) {        
        dir = BiDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while(pce != OFFBOARD) {
            if(pce != EMPTY) {
                if(IsBQ(pce) && PieceCol[pce] == side) {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }
    
    // kings
    for(index = 0; index < 8; ++index) {        
        pce = pos->pieces[sq + KiDir[index]];
        if(pce != OFFBOARD && IsKi(pce) && PieceCol[pce]==side) {
            return TRUE;
        }
    }
    
    return FALSE;
    
}
static
int SideValid(const int side) {
    return (side==WHITE || side == BLACK) ? 1 : 0;
}
int SqOnBoard(const int sq) {
    return FilesBrd[sq]==OFFBOARD ? 0 : 1;
}
static void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth) {
    int index = pos->posKey % pos->HashTable->numEntries;
    
    if( pos->HashTable->pTable[index].posKey == 0) {
        pos->HashTable->newWrite++;
    } else {
        pos->HashTable->overWrite++;
    }
    
    if(score > ISMATE) score += pos->ply;
        else if(score < -ISMATE) score -= pos->ply;
    
    pos->HashTable->pTable[index].move = move;
        pos->HashTable->pTable[index].posKey = pos->posKey;
    pos->HashTable->pTable[index].flags = flags;
    pos->HashTable->pTable[index].score = score;
    pos->HashTable->pTable[index].depth = depth;

}
static void TakeMove(S_BOARD *pos) {
    
    
    pos->hisPly--;
    pos->ply--;
    
    int move = pos->history[pos->hisPly].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);    
    
    
    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPas = pos->history[pos->hisPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    HASH_CA;

    pos->side ^= 1;
    HASH_SIDE;
    
    if(MFLAGEP & move) {
        if(pos->side == WHITE) {
            AddPiece(to-10, pos, bP);
        } else {
            AddPiece(to+10, pos, wP);
        }
    } else if(MFLAGCA & move) {
        switch(to) {
            case C1: MovePiece(D1, A1, pos); break;
            case C8: MovePiece(D8, A8, pos); break;
            case G1: MovePiece(F1, H1, pos); break;
            case G8: MovePiece(F8, H8, pos); break;
            default: break;
        }
    }
    
    MovePiece(to, from, pos);
    
    if(PieceKing[pos->pieces[from]]) {
        pos->KingSq[pos->side] = from;
    }
    
    int captured = CAPTURED(move);
    if(captured != EMPTY) {
        AddPiece(to, pos, captured);
    }
    
    if(PROMOTED(move) != EMPTY)   {
        ClearPiece(from, pos);
        AddPiece(from, pos, (PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
    }
    
}
static
void TakeNullMove(S_BOARD *pos) {

    pos->hisPly--;
    pos->ply--;

    if(pos->enPas != NO_SQ) HASH_EP;

    pos->castlePerm = pos->history[pos->hisPly].castlePerm;
    pos->fiftyMove = pos->history[pos->hisPly].fiftyMove;
    pos->enPas = pos->history[pos->hisPly].enPas;

    if(pos->enPas != NO_SQ) HASH_EP;
    pos->side ^= 1;
    HASH_SIDE;
 
}
static int TrapSearch(S_BOARD *pos, S_SEARCHINFO *info)
{
    printf("trap search starting \n");
        S_MOVELIST list[1];
    S_MOVELIST captures[1];
    int counter;
    int depth;
    int index;

    int bestMove=NOMOVE;
    int pvMove=NOMOVE;
    int bestScore=-INFINITE;
    int score=-INFINITE;
    int trapScore=-INFINITE;
    int bestTrapScore=-INFINITE;
    int trap=NOMOVE;

    index=pos->posKey % pos->HashTable->numEntries;
    if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
        bestMove=pos->HashTable->pTable[index].move;    
        bestScore=pos->HashTable->pTable[index].score;
        if(bestScore>500)
            return -INFINITE;
        depth=pos->HashTable->pTable[index].depth;
        pvMove=bestMove;
        trapScore=CheckCaptures(pos, pvMove, captures, bestScore);
        if (trapScore>bestTrapScore)
        {

            bestTrapScore=trapScore;
            trap=pvMove;
        }
        GenerateAllMoves(pos,list, FALSE);
    }else{
        return -INFINITE;
    }
    
    int count=list->count;
    for(counter=0;counter <= count;counter++)
    {
        if(info->stopped==TRUE)
            break;
        PopMove(list, pvMove);
        score=AlphaBeta(-INFINITE, INFINITE, depth, pos, info, TRUE, list, TRUE);
        
        if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
            pvMove=pos->HashTable->pTable[index].move;
        }
        if (score+THRESHOLD>bestScore)
        {
            //check captures
            trapScore=CheckCaptures(pos, pvMove, captures, bestScore);
            if(trapScore>bestTrapScore)
            {

                bestTrapScore=trapScore;
                trap=pvMove;
            }                
        }else
            break;
            
    }
    trapMove=trap;
    printf("trap search exitting with trapscore %d and trap move %s \n", bestTrapScore, PrMove(trapMove));
    return bestTrapScore;

}
static
void Uci_Loop() {
    
    while (TRUE) {
        memset(&line[0], 0, sizeof(line));
        if (!fgets(line, INPUTBUFFER, stdin))
        continue;

        if (line[0] == '\n')
        continue;

        if (!strncmp(line, "isready", 7)) {
            printf("readyok\n");
            continue;
        } else if (!strncmp(line, "position", 8)) {
            ParsePosition(line, pos);
        } else if (!strncmp(line, "ucinewgame", 10)) {
            ParsePosition("position startpos\n", pos);
        } else if (!strncmp(line, "go", 2)) {
            ParseGo(line, info, pos);
        } else if (!strncmp(line, "quit", 4)) {
            break;
        }
    }
    free(pos->HashTable->pTable);
}
static
void UpdateListsMaterial(S_BOARD *pos) {    
    
    int piece,sq,index,colour;
    
    for(index = 0; index < BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if(piece!=OFFBOARD && piece!= EMPTY) {
            colour = PieceCol[piece];
            
            if( PieceBig[piece] == TRUE) pos->bigPce[colour]++;
            if(piece==wK) pos->KingSq[WHITE] = sq;
            else if(piece==bK) pos->KingSq[BLACK] = sq;    
            else
                pos->material[colour] += PieceVal[piece];
            
            pos->pList[piece][pos->pceNum[piece]] = sq;
            pos->pceNum[piece]++;
            
                    
            if(piece==wP) {
                SETBIT(pos->pawns[WHITE],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            } else if(piece==bP) {
                SETBIT(pos->pawns[BLACK],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            }
        }
    }
}
int main()
{
    Init();    
    setbuf(stdin, NULL);
        setbuf(stdout, NULL);
    line=calloc(INPUTBUFFER, sizeof(char));    
        InitHashTable(pos->HashTable);

//loops
    while(1)
    {
        fgets(line, INPUTBUFFER, stdin);
        if(strncmp(line, "uci", 3)==0){
            printf("id name %s\n",NAME);
            printf("id author Kong\n");
            printf("uciok\n");
            Uci_Loop();
            continue;

        }
        if(strncmp(line, "console",7)==0)
        {
            Console_Loop();
            continue;
        }
        if(strncmp(line, "q", 1)==0)
        {    
            break;

        }
    }
    return 0;
}


