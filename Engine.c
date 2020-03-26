#include "config.h"
#include "defs.h"
#include "eval.c"
#include "LichessApi.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


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
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {

	
	if(depth == 0) {
		return Quiescence(alpha, beta, pos, info);
		// return EvalPosition(pos);
	}
	
	if(( info->nodes & 2047 ) == 0) {
		CheckUp(info);
	}
		
	info->nodes++;
	
	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply) {	
		return 0;
	}
	
	if(pos->ply > MAXDEPTH - 1) {
		return EvalPosition(pos);
	}
	
	int InCheck = SqAttacked(pos->KingSq[pos->side],pos->side^1,pos);
	
	if(InCheck == TRUE) {
		depth++;
	}
	
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
      
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE;
	int PvMove = ProbePvTable(pos);	
	
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
		
        if ( !MakeMove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -AlphaBeta( -beta, -alpha, depth-1, pos, info, TRUE);
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
				
				if(!(list->moves[MoveNum].move & MFLAGCAP)) {
					pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
					pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
				}
				
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
			if(!(list->moves[MoveNum].move & MFLAGCAP)) {
				pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}
		}		
    }
	
	if(Legal == 0) {
		if(InCheck) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}
	
	return alpha;
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
	
	ClearPvTable(pos->PvTable);	
	pos->ply = 0;
	
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}
void ClearPvTable(S_PVTABLE *table) {

  S_PVENTRY *pvEntry;

  for (pvEntry = table->pTable; pvEntry < table->pTable + table->numEntries; pvEntry++) {
    pvEntry->posKey = 0ULL;
    pvEntry->move = NOMOVE;
  }
}
int CountBits(U64 b) {
  int r;
  for(r = 0; b; r++, b &= b - 1);
  return r;
}
int FileRankValid(const int fr) {
	return (fr >= 0 && fr <= 7) ? 1 : 0;
}
void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list, int cap_only) {
	
	list->count = 0;	
	
	int pce = EMPTY;
	int side = pos->side;
	int sq = 0; int t_sq = 0;
	int pceNum = 0;
	int dir = 0;
	int index = 0;
	int pceIndex = 0;

		
	for(pceNum = 0; pceNum < pos->pceNum[side?bP:wP]; ++pceNum) {
		sq = side? pos->pList[bP][pceNum] : pos->pList[wP][pceNum];
		
		if(pos->pieces[sq + side? -10:10] == EMPTY && !cap_only) {
			AddBlackPawnMove(pos, sq, sq-10, list);
			AddWhitePawnMove(post, sq, sq+10, list);
			if(RanksBrd[sq] == (side: RANK_7:RANK_2) && pos->pieces[sq + (side:-20:20)] == EMPTY) {
				AddQuietMove(pos, MOVE(sq,(sq+side:-20:20),EMPTY,EMPTY,MFLAGPS),list);
			}
		} 
		
		if(!SQOFFBOARD(sq + (side?-9:9)) && PieceCol[pos->pieces[sq + (side?-9:9)]] == BLACK) {
			AddPawnCapMove(pos, sq, sq+(side?-9:9), pos->pieces[sq + (side?-9:9)], list);
		}  
		if(!SQOFFBOARD(sq + (side?-11:11)) && PieceCol[pos->pieces[sq + (side?-11:11)]] == BLACK) {
			AddPawnCapMove(pos, sq, sq+(side?-11:11), pos->pieces[sq + (side?-11:11)], list);
		} 
		
		if(pos->enPas != NO_SQ) {
			if(sq + (side?-9:9) == pos->enPas) {
				AddEnPassantMove(pos, MOVE(sq,sq + (side?-9:9),EMPTY,EMPTY,MFLAGEP), list);
			} 
			if(sq + (side?-11:11) == pos->enPas) {
				AddEnPassantMove(pos, MOVE(sq,sq + (side?-11:11),EMPTY,EMPTY,MFLAGEP), list);
			}
		}
	}
		
	if(pos->castlePerm & (side?BKCA:WKCA) && !cap_only) {
		if(pos->pieces[(side?F8:F1)] == EMPTY && pos->pieces[(side?G8:G1)] == EMPTY) {
			if(!SqAttacked((side?E8:E1),(side?WHITE:BLACK),pos) && !SqAttacked((side?F8:F1),(side?WHITE:BLACK),pos)) {
				AddQuietMove(pos, MOVE((side?E8:E1), (side?G8:G1), EMPTY, EMPTY, MFLAGCA), list);
			}
		}
	}
	if(pos->castlePerm & (side?BQCA:WQCA) && !cap_only) {
		if(pos->pieces[(side?D8:D1)] == EMPTY && pos->pieces[(side?C8:C1)] == EMPTY && pos->pieces[(side?B8:B1)]==EMPTY) {
			if(!SqAttacked((side?E8:E1),(side?WHITE:BLACK),pos) && !SqAttacked((side?F8:F1),(side?WHITE:BLACK),pos)) {
				AddQuietMove(pos, MOVE((side?E8:E1), (side?C8:C1), EMPTY, EMPTY, MFLAGCA), list);
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
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
			}
		}
				
		pce = LoopNonSlidePce[pceIndex++];
	}
}
int GetTimeMs() { 
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;
}

U64 GeneratePosKey(const S_BOARD *pos) {

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

void Init() {
	InitSq120To64();	
	InitBitMasks();
	InitHashKeys();
	InitFilesRanksBrd();
	InitMvvLva();
}


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
void InitFilesRanksBrd() {
	
	int index = 0;
	int file = FILE_A;
	int rank = RANK_1;
	int sq = A1;
	int sq64 = 0;
	
	for(index = 0; index < BRD_SQ_NUM; ++index) 
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

int InitMvvLva() {
	int Attacker;
	int Victim;
	for(Attacker = wP; Attacker <= bK; ++Attacker) {
		for(Victim = wP; Victim <= bK; ++Victim) {
			MvvLvaScores[Victim][Attacker] = VictimScore[Victim] + 6 - ( VictimScore[Attacker] / 100);
		}
	}		
}
void InitPvTable(S_PVTABLE *table) {  
  
    table->numEntries = PvSize / sizeof(S_PVENTRY);
    table->numEntries -= 2;
    if(table->pTable != NULL) free(table->pTable);
    table->pTable = (S_PVENTRY *) malloc(table->numEntries * sizeof(S_PVENTRY));
    ClearPvTable(table);
	
}
void InitSq120To64() {

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
static int IsRepetition(const S_BOARD *pos) {

	int index = 0;

	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly-1; ++index) {	
		ASSERT(index >= 0 && index < MAXGAMEMOVES);		
		if(pos->posKey == pos->history[index].posKey) {
			return TRUE;
		}
	}	
	return FALSE;
}
int MoveExists(S_BOARD *pos, const int move) {
	
	S_MOVELIST list[1];
    GenerateAllMoves(pos,list);
      
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
			default:	     break;
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
	PrintBoard(pos);	
}
static void PickNextMove(int moveNum, S_MOVELIST *list) {

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
}
int PieceValid(const int pce) {
	return (pce >= wP && pce <= bK) ? 1 : 0;
}
int PieceValidEmpty(const int pce) {
	return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}
int PopBit(U64 *bb) {
  U64 b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

void PrintBitBoard(U64 bb) {

	U64 shiftMe = 1ULL;
	
	int rank = 0;
	int file = 0;
	int sq = 0;
	int sq64 = 0;
	
	printf("\n");
	for(rank = RANK_8; rank >= RANK_1; --rank) {
		for(file = FILE_A; file <= FILE_H; ++file) {
			sq = FR2SQ(file,rank);	// 120 based		
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
int ProbePvTable(const S_BOARD *pos) {

	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);
	
	if( pos->PvTable->pTable[index].posKey == pos->posKey ) {
		return pos->PvTable->pTable[index].move;
	}
	
	return NOMOVE;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {

	ASSERT(CheckBoard(pos));
	
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
    GenerateAllCaps(pos,list);
      
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	Score = -INFINITE;
	int PvMove = ProbePvTable(pos);
	
	if( PvMove != NOMOVE ) {
		for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
			if( list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}
	
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
	
	if(alpha != OldAlpha) {
		StorePvMove(pos, BestMove);
	}
	
	return alpha;
}
int PopBit(U64 *bb) {
  U64 b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}
void PrintBitBoard(U64 bb) {

	U64 shiftMe = 1ULL;
	
	int rank = 0;
	int file = 0;
	int sq = 0;
	int sq64 = 0;
	
	printf("\n");
	for(rank = RANK_8; rank >= RANK_1; --rank) {
		for(file = FILE_A; file <= FILE_H; ++file) {
			sq = FR2SQ(file,rank);	// 120 based		
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
		pos->majPce[index] = 0;
		pos->minPce[index] = 0;
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
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	ClearForSearch(pos,info);
	
	// iterative deepening
	for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
							// alpha	 beta
		rootDepth = currentDepth;
		bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);
		
		if(info->stopped == TRUE) {
			break;
		}
		
		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];
		if(info->GAME_MODE == UCIMODE) {
			printf("info score cp %d depth %d nodes %ld time %d ",
				bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
		} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
			printf("%d %d %d %ld ",
				currentDepth,bestScore,(GetTimeMs()-info->starttime)/10,info->nodes);
		} else if(info->POST_THINKING == TRUE) {
			printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
				bestScore,currentDepth,info->nodes,GetTimeMs()-info->starttime);
		}
		if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
			pvMoves = GetPvLine(currentDepth, pos);	
			printf("pv");		
			for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
				printf(" %s",PrMove(pos->PvArray[pvNum]));
			}
			printf("\n");
		}
	}
	
	if(info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n",PrMove(bestMove));
	} else if(info->GAME_MODE == XBOARDMODE) {		
		printf("move %s\n",PrMove(bestMove));
		MakeMove(pos, bestMove);
	} else {	
		printf("\n\n***!! Vice makes move %s !!***\n\n",PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
	
}
int SqAttacked(const int sq, const int side, const S_BOARD *pos) {

	int pce,index,t_sq,dir;
	
	// pawns
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
int SideValid(const int side) {
	return (side==WHITE || side == BLACK) ? 1 : 0;
}
int SqOnBoard(const int sq) {
	return FilesBrd[sq]==OFFBOARD ? 0 : 1;
}
void StorePvMove(const S_BOARD *pos, const int move) {

	int index = pos->posKey % pos->PvTable->numEntries;
	ASSERT(index >= 0 && index <= pos->PvTable->numEntries - 1);
	
	pos->PvTable->pTable[index].move = move;
    pos->PvTable->pTable[index].posKey = pos->posKey;
}
void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info) {

	info->GAME_MODE = UCIMODE;
	
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	char line[INPUTBUFFER];
    printf("id name %s\n",NAME);
    printf("id author Bluefever\n");
    printf("uciok\n");    
	
	while (TRUE) {
		memset(&line[0], 0, sizeof(line));
        fflush(stdout);
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
            info->quit = TRUE;
            break;
        } else if (!strncmp(line, "uci", 3)) {
            printf("id name %s\n",NAME);
            printf("id author Bluefever\n");
            printf("uciok\n");
        }
		if(info->quit) break;
    }
}
void UpdateListsMaterial(S_BOARD *pos) {	
	
	int piece,sq,index,colour;
	
	for(index = 0; index < BRD_SQ_NUM; ++index) {
		sq = index;
		piece = pos->pieces[index];
		if(piece!=OFFBOARD && piece!= EMPTY) {
			colour = PieceCol[piece];
			
		    if( PieceBig[piece] == TRUE) pos->bigPce[colour]++;
		    if( PieceMin[piece] == TRUE) pos->minPce[colour]++;
		    if( PieceMaj[piece] == TRUE) pos->majPce[colour]++;
			
			pos->material[colour] += PieceVal[piece];
			
			pos->pList[piece][pos->pceNum[piece]] = sq;
			pos->pceNum[piece]++;
			
			if(piece==wK) pos->KingSq[WHITE] = sq;
			if(piece==bK) pos->KingSq[BLACK] = sq;	
			
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
AllInit();	
	
	S_BOARD pos[1];
    	S_SEARCHINFO info[1];   
	pos->PvTable->pTable = NULL;
    	InitPvTable(pos->PvTable);
	setbuf(stdin, NULL);
 	setbuf(stdout, NULL);
	
	char line[256];
	while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci",3)) {			
			Uci_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard",6))	{
			XBoard_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "vice",4))	{
			Console_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit",4))	{
			break;
		}
	}
	
	free(pos->PvTable->pTable);

//uci loop
	return 0;
}
