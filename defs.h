#ifndef DEF_H
#define DEF_H

#include "config.h"
#include "stdlib.h"


typedef unsigned long long U64;


#define BRD_SQ_NUM 120
#define INFINITE 30000
#define INPUTBUFFER 2560
#define MATE 29000
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
#define ENDGAME (1 * PieceVal[wR] + 2 * PieceVal[wN] + 8 * PieceVal[wP])
#define FR2SQ(f,r) ( (21 + (f) ) + ( (r) * 10 ) ) 
#define HASH_CA (pos->posKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_EP (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPas)]))
#define HASH_PCE(pce,sq) (pos->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_SIDE (pos->posKey ^= (SideKey))
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

U64 ClearMask[64];
U64 SetMask[64];
int Sq64ToSq120[64];
int Sq120ToSq64[BRD_SQ_NUM];
/*globals across files*/

extern U64 BlackBackwardsMask[64];
extern U64 BlackOutpostMask[64];
extern U64 BlackPassedMask[64];
extern U64 FileBBMask[8];
extern U64 IsolatedMask[64];
extern U64 RankBBMask[8];
extern U64 WhiteBackwardsMask[64];
extern U64 WhiteOutpostMask[64];
extern U64 WhitePassedMask[64];

extern const int BiDir[4];
extern const int KiDir[8];
extern const int KnDir[8];
extern const int RkDir[4];

extern int FilesBrd[BRD_SQ_NUM];
extern const int PieceVal[13];
extern const int Mirror64[64];
extern int RanksBrd[BRD_SQ_NUM];

/* FUNCTIONS */
static void AddCaptureMove( const S_BOARD *pos, int move, S_MOVELIST *list );
static void AddEnPassantMoe( const S_BOARD *pos, int move, S_MOVELIST *list );
static void AddPawnCapMove( const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list ) ;
static void AddPawnMove( const S_BOARD *pos, const int from, const int to, S_MOVELIST *list );
static void AddPiece(const int sq, S_BOARD *pos, const int pce);
static void AddQuietMove( const S_BOARD *pos, int move, S_MOVELIST *list );
static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull); 
int CheckKingSquare(const S_BOARD *pos, int sq, int King, int side);
static void CheckUp(S_SEARCHINFO *info);
static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info);
static void ClearPiece(const int sq, S_BOARD *pos);
static void ClearPvTable(S_PVTABLE *table);
static void Console_Loop();
static int CountBits(U64 b);
int EvalBlackBishop(const S_BOARD *pos);
int EvalBlackKing(const S_BOARD *pos);
int EvalBlackKingPawns(const S_BOARD *pos);
int EvalBlackKnight(const S_BOARD *pos);
int EvalBlackPawns(const S_BOARD *pos);
int EvalBlackRook(const S_BOARD *pos);
static int EvalPosition(const S_BOARD *pos);
int EvalWhiteBishop(const S_BOARD *pos);
int EvalWhiteKing(const S_BOARD *pos);
int EvalWhiteKingPawns(const S_BOARD *pos);
int EvalWhiteKnight(const S_BOARD *pos);
int EvalWhitePawns(const S_BOARD *pos);
int EvalWhiteRook(const S_BOARD *pos);
static int FileRankValid(const int fr);
static void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list, int cap_only);
static U64 GeneratePosKey(const S_BOARD *pos);
static int GetPvLine(const int depth, S_BOARD *pos);
static int GetTimeMs();
static void Init();
static void InitBitMasks();
static void InitEvalMasks();
static void InitFilesRanksBrd();
static void InitHashKeys();
static int InitMvvLva();
static void InitPvTable(S_PVTABLE *table);
static void InitSq120To64();
static int InputWaiting();
static int IsRepetition(const S_BOARD *pos);
static int MakeMove(S_BOARD *pos, int move);
static void MakeNullMove(S_BOARD *pos);
static int MoveExists(S_BOARD *pos, const int move);
static void MovePiece(const int from, const int to, S_BOARD *pos);
static int ParseFen(char *fen, S_BOARD *pos);
static void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos);
static void ParsePosition(char* lineIn, S_BOARD *pos);
static int ParseMove(char *ptrChar, S_BOARD *pos);
static void PickNextMove(int moveNum, S_MOVELIST *list);
static int PieceValid(const int pce);
static int PieceValidEmpty(const int pce);
static int PopBit(U64 *bb);
void PrintBitBoard(U64 bb);
void PrintBoard(const S_BOARD *pos);
static void PrintPositionalEvals(S_BOARD *pos);
static char * PrMove(const int move);
static int ProbePvTable(const S_BOARD *pos);
static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info);
static void ReadInput(S_SEARCHINFO *info);
static void ResetBoard(S_BOARD *pos); 
static void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);
int SqAttacked(const int sq, const int side, const S_BOARD *pos);
static int SideValid(const int side);
int SqOnBoard(const int sq);
static void StorePvMove(const S_BOARD *pos, const int move);
static void TakeMove(S_BOARD *pos);
static void TakeNullMove(S_BOARD *pos);
static void Uci_Loop();
static void UpdateListsMaterial(S_BOARD *pos);
#endif
