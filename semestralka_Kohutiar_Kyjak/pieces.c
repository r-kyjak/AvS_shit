#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pieces.h"

/*funkcia na inicializaciu sachovnice,
 nastavuje defaultne hodnoty pre figurky a policka a nastavuje typy figurok na zaciatocnych poziciach
 */
void initBoard(BOARD* chessBoard){
    lostBlackPieces.size = 0;
    lostWhitePieces.size = 0;
    int white = 0;
    int black = 0;
	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){
                    PIECE* p = calloc(1, sizeof(PIECE));
			if(i == 1){
                            p->color = 1;//1 je biela
                            p->onBoard = true;
                            p->type = PESIAK;
                            p->movedAlready = false;
                            p->xPos = i;
                            p->yPos = j;
                            *(*(chessBoard->policka + i) +j) = p; 
                            *(chessBoard->whitePieces + white++) = p;
                        } else if(i == 0){
                            p->color = 1;//1 je biela
                            p->onBoard = true;
                            p->movedAlready = false;
                            p->xPos = i;
                            p->yPos = j;
                            *(*(chessBoard->policka + i) +j) = p; 
                            *(chessBoard->whitePieces + white++) = p;
                        } else if(i == 7){
                            p->color = 0; //0 je cierna
                            p->onBoard = true;
                            p->movedAlready = false;
                            p->xPos = i;
                            p->yPos = j;
                            *(*(chessBoard->policka + i) +j) = p; 
                            *(chessBoard->blackPieces + black++) = p;
                        } else if(i == 6){
                            p->color = 0; //0 je cierna
                            p->onBoard = true;
                            p->type = PESIAK;
                            p->movedAlready = false;
                            p->xPos = i;
                            p->yPos = j;
                            *(*(chessBoard->policka + i) +j) = p; 
                            *(chessBoard->blackPieces + black++) = p;
                        } else if(i != 7 && i != 0 && i != 1 && i != 6){
                            p->color = 3; //3 je prazne pole
                            p->onBoard = true;
                            p->type = BLOK;
                            p->xPos = i;
                            p->yPos = j;
                            p->movedAlready = true;
                            *(*(chessBoard->policka + i) +j) = p;
			}
		}
	}
	chessBoard->policka[7][0]->type = VEZA;
	chessBoard->policka[7][1]->type = JAZDEC;
	chessBoard->policka[7][2]->type = STRELEC;
	chessBoard->policka[7][3]->type = KRALOVNA;
	chessBoard->policka[7][4]->type = KRAL;
        chessBoard->blackKing = chessBoard->policka[7][4];
	chessBoard->policka[7][5]->type = STRELEC;
	chessBoard->policka[7][6]->type = JAZDEC;
	chessBoard->policka[7][7]->type = VEZA;
	
	chessBoard->policka[0][0]->type = VEZA;
	chessBoard->policka[0][1]->type = JAZDEC;
	chessBoard->policka[0][2]->type = STRELEC;
	chessBoard->policka[0][3]->type = KRALOVNA;
	chessBoard->policka[0][4]->type = KRAL;
        chessBoard->whiteKing = chessBoard->policka[0][4];
	chessBoard->policka[0][5]->type = STRELEC;
	chessBoard->policka[0][6]->type = JAZDEC;
	chessBoard->policka[0][7]->type = VEZA;
}
/*vykresli sachovnicu a pozicie jednotlivych figurok a ich prislusnost hracom
 */
void printBoard(BOARD* chessBoard){
    printf("    [0 ][1 ][2 ][3 ][4 ][5 ][6 ][7 ]\n");
	for(int i = 0; i < BOARD_SIZE; i++){
            printf("[%c ]", 'A' + i);
		for(int j = 0; j < BOARD_SIZE; j++){
                    if(chessBoard->policka[i][j]->color == 3){
                        printf("[%c ]", chessBoard->policka[i][j]->type);
                    } else {
                        printf("[%c%d]", chessBoard->policka[i][j]->type, chessBoard->policka[i][j]->color == 1 ? 1 : 2);
                    }
		}
		printf("\n");
	}
        printf("------------------------------------\n");
}