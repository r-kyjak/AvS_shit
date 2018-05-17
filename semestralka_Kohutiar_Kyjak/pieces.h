#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define NUMBER_OF_PIECES 16
#define PESIAK 'p'
#define VEZA 'v'
#define JAZDEC 'j'
#define STRELEC 's'
#define KRALOVNA 'q'
#define KRAL 'k'
#define BLOK ' '

/*
 * Struktura policka na sachovnici, obsahuje atributy popisujuce obsadenie hracom, 
 * typ, nachadzanie sa na sachovnici, 
 * prvy pohyb pesiakov a poziciu
 */
typedef struct piece{
	char type;
	unsigned short int color;
	bool onBoard;
	bool movedAlready;
        short int xPos;
        short int yPos;
} PIECE;

/*struktura obsahujuca posle vyhodenych figurok hraca a ich pocet
 */
typedef struct lost{
	PIECE* pieces[NUMBER_OF_PIECES];
	unsigned short int size;
} LOST;

/*struktura obsahujuca pole vsetkych policok, pole bielych a ciernych figurok a smerniky na kralov
 */
typedef struct board{
	PIECE* policka[BOARD_SIZE][BOARD_SIZE];
        PIECE* whitePieces[NUMBER_OF_PIECES];
        PIECE* blackPieces[NUMBER_OF_PIECES];
        PIECE* whiteKing;
        PIECE* blackKing;
} BOARD;

/*deklarovanie struktur stratenych figurok pre hracov
 */
LOST lostBlackPieces;
LOST lostWhitePieces;

void initBoard(BOARD* chessBoard);
void printBoard(BOARD* chessBoard);