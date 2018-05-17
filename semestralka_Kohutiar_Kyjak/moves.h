#include "pieces.h"

void printInvalidMove();
bool skontrolujPohybPesiaka(BOARD* chessBoard, PIECE* p, int fromX, int fromY, int toX, int toY, int pocetMoznychKrokov, bool checkOnly,bool ignoreColor);
bool vyhod(BOARD* chessBoard,PIECE* piece,int x, int y, int toX, int toY);
bool movePawn(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool moveJazdec(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool moveVeza(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool moveKral(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool moveStrelec(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool move(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor);
bool checkForChess(BOARD* chessBoard, PIECE* piece, bool ignoreColor, int optionalKingColor, bool ibaCekujem);
bool checkMat(BOARD* chessBoard, PIECE* piece, bool* matAleboPat);

/*smerniky na funkcie
 */
bool (*PmovePawn)(BOARD*, PIECE*, int, int, int, int, bool, bool);
bool (*PmoveJazdec)(BOARD*, PIECE*, int, int, int, int, bool, bool);
bool (*PmoveVeza)(BOARD*, PIECE*, int, int, int, int, bool, bool);
bool (*PmoveKral)(BOARD*, PIECE*, int, int, int, int, bool, bool);
bool (*PmoveStrelec)(BOARD*, PIECE*, int, int, int, int, bool, bool);
