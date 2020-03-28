
#include "config.h"
#include "stdlib.h"

#ifndef DEFS_H
#define DEFS_H

typedef unsigned long long U64;


#define BRD_SQ_NUM 120
#define NOMOVE 0
#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK  };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };
enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
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
enum { FALSE, TRUE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };
typedef struct {
	int move;
	int score;
} S_MOVE;

typedef struct {
	S_MOVE moves[MAXPOSITIONMOVES];
	int count;
} S_MOVELIST;

typedef struct {
	U64 posKey;
	int move;
} S_PVENTRY;

typedef struct {
	S_PVENTRY *pTable;
	int numEntries;
} S_PVTABLE;

typedef struct {
	
	int move;
	int castlePerm;
	int enPas;
	int fiftyMove;
	U64 posKey;

} S_UNDO;

typedef struct {

	int pieces[BRD_SQ_NUM];
	U64 pawns[3];
		
	int KingSq[2];
	
	int side;
	int enPas;
	int fiftyMove;
	
	int ply;
	int hisPly;
	
	int castlePerm;
	
	U64 posKey;
	
	int pceNum[13];
	int bigPce[2];
	int majPce[2];
	int minPce[2];
	int material[2];
	
	S_UNDO history[MAXGAMEMOVES];
	
	// piece list
	int pList[13][10];	

	S_PVTABLE PvTable[1];	
	int PvArray[MAXDEPTH];
	
	int searchHistory[13][BRD_SQ_NUM];
	int searchKillers[2][MAXDEPTH];
	
} S_BOARD;

typedef struct {

	int starttime;
	int stoptime;
	int depth;
	int timeset;
	int movestogo;
	
	long nodes;
	
	int quit;
	int stopped;

	float fh;
	float fhf;

	int GAME_MODE;
	int POST_THINKING;

} S_SEARCHINFO;	

#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))
#define HASH_PCE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
#define INFINITE 30000
#define INPUTBUFFER 2560
#define MATE 29000

/* GAME MOVE */

/*                         	                        
0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/

#define CAPTURED(m) (((m)>>14) & 0xF)
#define FROMSQ(m) ((m) & 0x7F)
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7C000
#define MFLAGEP 0x40000
#define MFLAGPROM 0xF00000
#define MFLAGPS 0x80000
#define PROMOTED(m) (((m)>>20) & 0xF)
#define TOSQ(m) (((m)>>7) & 0x7F)

/* MACROS */

#define CLRBIT(bb,sq) ((bb) &= ClearMask[(sq)])
#define CNT(b) CountBits(b)
#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) ) 
#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsKi(p) (PieceKing[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define MIRROR64(sq) (Mirror64[(sq)])
#define MOVE(f,t,ca,pro,fl) ( (f) | ((t) << 7) | ( (ca) << 14 ) | ( (pro) << 20 ) | (fl))
#define POP(b) PopBit(b)
#define RAND_64 ((U64)rand() | \
		(U64)rand() << 15 | \
		(U64)rand() << 30 | \
		(U64)rand() << 45 | \
		((U64)rand() & 0xf) << 60 )
#define SETBIT(bb,sq) ((bb) |= SetMask[(sq)])
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define SQOFFBOARD(sq) (FilesBrd[(sq)]==OFFBOARD)


/* GLOBALS */
const int BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};
const int Castle=40;
const int DoubledPawns=-5;
const int KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int Mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};
const int PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};
const int PawnBackwards= -5;
const int PawnIsolated = -10;
const int PawnPassed[8] = { 0, 5, 10, 20, 35, 60, 100, 200 }; 
const int RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};
const int RookOpenFile = 5;


const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10,	1, 10, -9, -11, 11, 9 };
const int KnDir[8] = { -8, -19,	-21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10,	1, 10 };


const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};
const int LoopNonSlideIndex[2] = { 0, 3 };
const int LoopNonSlidePce[6] = {
 wN, wK, 0, bN, bK, 0
};
const int LoopSlideIndex[2] = { 0, 4 };
const int LoopSlidePce[8] = {
 wB, wR, wQ, 0, bB, bR, bQ, 0
};

static int MvvLvaScores[13][13];
const int NumDir[13] = {
 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
};

const int PceDir[13][8] = {
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ 0, 0, 0, 0, 0, 0, 0 },
	{ -8, -19,	-21, -12, 8, 19, 21, 12 },
	{ -9, -11, 11, 9, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, 0, 0, 0, 0 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 },
	{ -1, -10,	1, 10, -9, -11, 11, 9 }
};
const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };

U64 BlackBackwardsMask[64];
U64 BlackPassedMask[64];
U64 CastleKeys[16];
U64 ClearMask[64];
U64 FileBBMask[8];
U64 IsolatedMask[64];
U64 PieceKeys[13][120];
U64 RankBBMask[8];
U64 SetMask[64];
U64 SideKey;
int Sq64ToSq120[64];
int Sq120ToSq64[BRD_SQ_NUM];
U64 WhiteBackwardsMask[64];
U64 WhitePassedMask[64];

char FileChar[] = "abcdefgh";
char PceChar[] = ".PNBRQKpnbrqk";
char RankChar[] = "12345678";
char SideChar[] = "wb-";

int PieceBig[13] = { FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, TRUE, TRUE };
int PieceCol[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
int PieceMaj[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE };
int PieceMin[13] = { FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE };
int PiecePawn[13] = { FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE };
int PieceVal[13]= { 0, 100, 325, 350, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000  };

int FilesBrd[BRD_SQ_NUM];
int RanksBrd[BRD_SQ_NUM];

int PieceBishopQueen[13] = { FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE };
int PieceKing[13] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE };
int PieceKnight[13] = { FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE };
int PieceRookQueen[13] = { FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE };
int PieceSlides[13] = { FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE };

char *line;
const int PvSize = 0x100000 * 2;
int rootDepth;
S_BOARD pos[1];
S_SEARCHINFO info[1];

/* FUNCTIONS */
void AddCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list );
void AddEnPassantMoe( const S_BOARD *pos, int move, S_MOVELIST *list );
void AddPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) ;
void AddPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list );
void AddPiece(const int sq, S_BOARD *pos, const int pce);
void AddQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list );
int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info);
void CheckUp(S_SEARCHINFO *info);
void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info);
void ClearPiece(const int sq, S_BOARD *pos);
void ClearPvTable(S_PVTABLE *table);
void Console_Loop();
int CountBits(U64 b);
int EvalPosition(const S_BOARD *pos);
int FileRankValid(const int fr);
void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list, int cap_only);
U64 GeneratePosKey(const S_BOARD *pos);
int GetPvLine(const int depth, S_BOARD *pos);
int GetTimeMs();
void Init();
void InitBitMasks();
void InitEvalMasks();
void InitFilesRanksBrd();
void InitHashKeys();
int InitMvvLva();
void InitPvTable(S_PVTABLE *table);
void InitSq120To64();
int InputWaiting();
int IsRepetition(const S_BOARD *pos);
int MakeMove(S_BOARD *pos, int move);
int MoveExists(S_BOARD *pos, const int move);
void MovePiece(const int from, const int to, S_BOARD *pos);
int ParseFen(char *fen, S_BOARD *pos);
void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos);
void ParsePosition(char* lineIn, S_BOARD *pos);
int ParseMove(char *ptrChar, S_BOARD *pos);
void PickNextMove(int moveNum, S_MOVELIST *list);
int PieceValid(const int pce);
int PieceValidEmpty(const int pce);
int PopBit(U64 *bb);
void PrintBitBoard(U64 bb);
void PrintBoard(const S_BOARD *pos);
void PrintPositionalEvals(S_BOARD *pos);
char * PrMove(const int move);
int PopBit(U64 *bb);
void PrintBitBoard(U64 bb);
int ProbePvTable(const S_BOARD *pos);
int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info);
void ReadInput(S_SEARCHINFO *info);
void ResetBoard(S_BOARD *pos); 
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);
int SqAttacked(const int sq, const int side, const S_BOARD *pos);
int SideValid(const int side);
int SqOnBoard(const int sq);
void StorePvMove(const S_BOARD *pos, const int move);
void TakeMove(S_BOARD *pos);
void Uci_Loop();
void UpdateListsMaterial(S_BOARD *pos);
#endif
