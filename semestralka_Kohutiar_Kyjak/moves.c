#include "moves.h"

bool (*PmovePawn)(BOARD*, PIECE*, int, int, int, int, bool, bool) = movePawn;
bool (*PmoveJazdec)(BOARD*, PIECE*, int, int, int, int, bool, bool) = moveJazdec;
bool (*PmoveVeza)(BOARD*, PIECE*, int, int, int, int, bool, bool) = moveVeza;
bool (*PmoveKral)(BOARD*, PIECE*, int, int, int, int, bool, bool) = moveKral;
bool (*PmoveStrelec)(BOARD*, PIECE*, int, int, int, int, bool, bool) = moveStrelec;

void printInvalidMove(){
    printf("Chyba, nespravny tah!\n");
}

/*funkcia kontrolujuca pohyb pesiaka,
 vypocita o kolko sa pesiak chce pohnut, skontroluje ci je na defaultnej pozicii a jeho prislusnost hracovi
 @param chessBoard obsahuje smernik na sachovnicu
 @param p je smernik na figurku
 @param fromX, fromY, toX a toY su suradnice pozicii (odkial a kam)
 @param pocetMoznychKrokov obsahuje o kolko sa maximalne môze pesiak pohnut
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak je pohyb na danu poziciu mozny 
 */
bool skontrolujPohybPesiaka(BOARD* chessBoard, PIECE* p, int fromX, int fromY, int toX, int toY, int pocetMoznychKrokov, bool checkOnly, bool ignoreColor){
	if(p->type != PESIAK){
            return false;
	} 
        int oKolko = 0;
        if(p->color == 1){
            oKolko = toY - fromY;
            if(toY > fromY && toY < 8 && toY >= 0 && toX < 8 && toX >= 0){
                if(oKolko == 1){
                    bool p1 = (toX == 0 ? true : chessBoard->policka[toY][toX-1]->color != 0);
                    bool p2 =  (toX == 7 ? true : chessBoard->policka[toY][toX+1]->color != 0);
                    if(chessBoard->policka[toY][toX]->type == BLOK && toX == fromX && p1 && p2 && !checkOnly){
                        return true;
                    } else if(abs(toX-fromX) == 1 && chessBoard->policka[toY][toX]->color == 0){
                        return true;//skontroluj ci ide sikmo => vyhadzujes, inak zle a nemozes ist
                    } else if((p1 || p2) && toX == fromX){
                        if(!checkOnly){
                            printf("zly tah, musis vyhadzovat!\n");
                        }
                        return false;
                    } else {
                        return false;
                    }
                } else if(oKolko == 2 && !p->movedAlready){
                    if((toX == 0 ? false : chessBoard->policka[toY-1][toX-1]->color == 0) && (toX == 7 ? false : chessBoard->policka[toY-1][toX+1]->color == 0)){
                        if(!checkOnly){
                            printf("zly tah, musis vyhadzovat!\n");
                        }
                        return false;
                    } else if(chessBoard->policka[toY][toX]->type == BLOK && toX == fromX && chessBoard->policka[toY-1][toX]->type == BLOK){
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
	} else {
            oKolko = fromY - toY;
             if(toY < fromY && toY < 8 && toY >= 0 && toX < 8 && toX >= 0){
                if(oKolko == 1){
                    if(chessBoard->policka[toY][toX]->type == BLOK && toX == fromX && (toX == 0 ? true : chessBoard->policka[toY][toX-1]->color != 1) && (toX == 7 ? true : chessBoard->policka[toY][toX+1]->color != 1) && !checkOnly){
                        return true;
                    } else if(chessBoard->policka[toY][toX]->color == 1 && (fromX - toX) == 1){
                        //skontroluj ci ide sikmo => vyhadzujes, inak zle a nemozes ist
                        return true;
                    } else if(((toX == 0 ? false : chessBoard->policka[toY][toX-1]->color != 1) || (toX == 7 ? false : chessBoard->policka[toY][toX+1]->color != 1)) && toX == fromX){
                        if(!checkOnly){
                            printf("zly tah, musis vyhadzovat!\n");
                        }
                        return false;
                    } else {
                        return false;
                    }
                } else if(oKolko == 2 && !p->movedAlready){
                    if((toX == 0 ? false : chessBoard->policka[toY+1][toX-1]->color == 1) && (toX == 7 ? false : chessBoard->policka[toY+1][toX+1]->color == 1)){
                        if(!checkOnly){
                            printf("zly tah, musis vyhadzovat!\n");
                        }
                        return false;
                    } else if(chessBoard->policka[toY][toX]->type == BLOK && toX == fromX && chessBoard->policka[toY+1][toX]->type == BLOK){
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
}

/*funkcia na vykonanie pohybu, ide o samotne vyhadzovanie alebo posunutie figurky na danu poziciu
 * funkcia vykonava kontrolu pohybu ci nedojde k sachu vlastneho krala
 * po vykonani pohybu kontroluje ci nedojde k sachu nepriatelskeho krala
 * @param chessBoard obsahuje smernik na sachovnicu
 * @param p je smernik na figurku
 * @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 */
bool vyhod(BOARD* chessBoard,PIECE* piece,int x, int y, int toX, int toY){
    //ak sa nehybes na blok tak vyhadzujes
    if(chessBoard->policka[toX][toY]->type != BLOK){
        if(chessBoard->policka[toX][toY]->color == 0){
            //uklada figurku na pozicii na ktoru ideme
            PIECE* povodny = chessBoard->policka[toX][toY];
            //posunie figurku a zmeni atributy
            piece->xPos = toX;
            piece->yPos = toY;
            chessBoard->policka[toX][toY] = piece;
            //vytvorenie prazneho policka
            PIECE* p = calloc(1, sizeof(PIECE));
            //nastav atributy
            p->color = 3;
            p->onBoard = true;
            p->type = BLOK;
            p->movedAlready = true;
            p->xPos = y;
            p->yPos = x;
            chessBoard->policka[y][x] = p;
            povodny->onBoard = false;
            //kontrola ci pohybom nesposobime vlastny sach
            if(checkForChess(chessBoard, chessBoard->whiteKing, false, 3, false)){
                //ak ano, revertnu pohyb
                povodny->onBoard = true;
                piece->xPos = y;
                piece->yPos = x;
                chessBoard->policka[toX][toY] = povodny;
                chessBoard->policka[y][x] = piece;
                free(p);
                return false;
            } else {
                //ak nie, vyhodi figurku a skontroluje sach na nepriatelskeho krala
                piece->movedAlready = true;
                *(lostBlackPieces.pieces + lostBlackPieces.size++) = povodny;
                checkForChess(chessBoard, chessBoard->blackKing, false, 3, false);
                return true;
            }
        } else {
            PIECE* povodny = chessBoard->policka[toX][toY];
            piece->xPos = toX;
            piece->yPos = toY;
            chessBoard->policka[toX][toY] = piece;
            PIECE* p = calloc(1, sizeof(PIECE));
            //nastav atributy
            p->color = 3;
            p->onBoard = true;
            p->type = BLOK;
            p->movedAlready = true;
            p->xPos = y;
            p->yPos = x;
            chessBoard->policka[y][x] = p;
            povodny->onBoard = false;
            if(checkForChess(chessBoard, chessBoard->blackKing, false, 3, false)){
                povodny->onBoard = true;
                piece->xPos = y;
                piece->yPos = x;
                chessBoard->policka[toX][toY] = povodny;
                chessBoard->policka[y][x] = piece;
                free(p);
                return false;
            } else {
                piece->movedAlready = true;
                *(lostWhitePieces.pieces + lostWhitePieces.size++) = povodny;
                checkForChess(chessBoard, chessBoard->whiteKing, false, 3, false);
                return true;
            }
        }
    } else {
        if(chessBoard->policka[y][x]->color == 0){
            PIECE* povodny = chessBoard->policka[toX][toY];
            piece->xPos = toX;
            piece->yPos = toY;
            chessBoard->policka[toX][toY] = piece;
            chessBoard->policka[y][x] = povodny;
            povodny->xPos = y;
            povodny->yPos = x;
            if(checkForChess(chessBoard, chessBoard->blackKing, false, 3, true)){
                piece->xPos = y;
                piece->yPos = x;
                povodny->xPos = toY;
                povodny->yPos = toX;
                chessBoard->policka[toX][toY] = povodny;
                chessBoard->policka[y][x] = piece;
                return false;
            } else {
                piece->movedAlready = true;
                povodny->onBoard = true;
                checkForChess(chessBoard, chessBoard->whiteKing, false, 3, false);
                return true;
            }
        } else {
            PIECE* povodny = chessBoard->policka[toX][toY];
            piece->xPos = toX;
            piece->yPos = toY;
            chessBoard->policka[toX][toY] = piece;
            chessBoard->policka[y][x] = povodny;
            povodny->xPos = y;
            povodny->yPos = x;
            if(checkForChess(chessBoard, chessBoard->whiteKing, false, 3, true)){
                piece->xPos = y;
                piece->yPos = x;
                povodny->xPos = toY;
                povodny->yPos = toX;
                chessBoard->policka[toX][toY] = povodny;
                chessBoard->policka[y][x] = piece;
                return false;
            } else {
                piece->movedAlready = true;
                povodny->onBoard = true;
                checkForChess(chessBoard, chessBoard->blackKing, false, 3, false);
                return true;
            }
        }
    }
}

/*skontroluje moznost pohybu pesiaka a ak sa da tak s nim pohne
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny 
 */
bool movePawn(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
	if(skontrolujPohybPesiaka(chessBoard, piece, x, y, toX, toY, piece->movedAlready ? 1 : 2, checkOnly, ignoreColor)){
            if(!checkOnly){
                return vyhod(chessBoard, piece, x, y, toY, toX);
            }
            piece->movedAlready = true;
            return true;
	} else {
            if(!checkOnly){
                printInvalidMove();
            }
            return false;
        }
}


/*funckia kontrolujuca smer pohybu kona, ak ide o spravne suradnice vykona pohyb ak sa nehybeme na vlastnu figurku
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny  
 */
bool moveJazdec(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
    bool colorOK = true;
    if(!ignoreColor){
        colorOK = piece->color != chessBoard->policka[toY][toX]->color;
    }
    if(piece->type != JAZDEC){
            return false;
    }
    if((toY == y+2 || toY == y-2) && (toX == x+1 || toX == x-1) && colorOK){
        if(!checkOnly){
            return vyhod(chessBoard, piece, x, y, toY, toX);
        }
        return true;
    } else if((toX == x+2 || toX == x-2) && (toY == y+1 || toY == y-1) && colorOK){
        if(!checkOnly){
            return vyhod(chessBoard, piece, x, y, toY, toX);
        }
        return true;
    }else {
        if(!checkOnly){
            printInvalidMove();
        }
        return false;
    }
}
/*funkcia na pohyb veze, kontroluje smer pohybu a ci v drahe pohybu nestoji panak ktory by blokoval cestu + kontroluje ci sa nehybeme na vlastneho
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny  
*/
bool moveVeza(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
    bool colorOK = true;
    if(!ignoreColor){
        colorOK = piece->color != chessBoard->policka[toY][toX]->color;
    }
    if(piece->type != VEZA && piece->type != KRALOVNA){
        return false;
    }
    if(toX == x && colorOK){
        if(toY < y){
            for(int i = y-1; i >= toY; i--){
                if(chessBoard->policka[i][x]->color != 3 && i > toY){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                }
            }
            if(!checkOnly){
                return vyhod(chessBoard, piece, x, y, toY, toX);
            }
            return true;
        } else {
            for(int i = y+1; i <= toY; i++){
                if(chessBoard->policka[i][x]->color != 3 && i < toY){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                }
            }
            if(!checkOnly){
                return vyhod(chessBoard, piece, x, y, toY, toX);
            }
            return true;
        }
    } else if(toY == y && colorOK){
        if(toX < x){
            for(int i = x-1; i >= toX; i--){
                if(chessBoard->policka[y][i]->color != 3 && i > toX){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                }
            }
            if(!checkOnly){
                return vyhod(chessBoard, piece, x, y, toY, toX);
            }
            return true;
        } else {
            for(int i = x+1; i <= toX; i++){
                if(chessBoard->policka[y][i]->color != 3 && i < toX){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                } 
            }
            if(!checkOnly){
                return vyhod(chessBoard, piece, x, y, toY, toX);
            }
            return true;
        }
    } else {
        if(!checkOnly){
            printInvalidMove();
        }
        return false;
    }
}

/*funkcia na pohyb krala, kontroluje smer pohybu a pokusi sa pohnut
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny  
 */
bool moveKral(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
    bool colorOK = true;
    if(!ignoreColor){
        colorOK = piece->color != chessBoard->policka[toY][toX]->color;
    }
    if(piece->type != KRAL){
            return false;
    }
    if(((toY == y && (toX == x-1 || toX == x+1)) || (toX == x && (toY == y+1 || toY == y-1))) && colorOK){
        if(!checkOnly){
            return vyhod(chessBoard, piece, x, y, toY, toX);
        }
        return true;
    } else if(((toY == y+1 && (toX == x-1 || toX == x+1)) || (toY == y-1 && (toX == x-1 || toX == x+1)))){
        if(!checkOnly){
            return vyhod(chessBoard, piece, x, y, toY, toX);
        }
        return true;
    }else {
        if(!checkOnly){
            printInvalidMove();
        }
        return false;
    }
}

/*funkcia na pohyb strelca, kontroluje smer pohybu a ci v smere pohybu nestoji ina figurka ktora by mu branila
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny  
 */
bool moveStrelec(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
    bool colorOK = false;
    if(!ignoreColor){
        colorOK = piece->color == chessBoard->policka[toY][toX]->color;
    }
    if((piece->type != STRELEC && piece->type != KRALOVNA) || colorOK){
        if(!checkOnly){
            printInvalidMove();
        }
        return false;
    }
    
    if(toX-x == toY-y){
        if(toX>x){
            for(int i = x+1, j = y+1; i <= toX && j <= toY; i++, j++){//doprava dole
                if(i != toX && j != toY && chessBoard->policka[j][i]->color != 3){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                } else if(i == toX && j == toY) {
                    if(!checkOnly){
                        return vyhod(chessBoard, piece, x, y, toY, toX);
                    }
                    return true;
                }
            }
        } else {
            for(int i = x-1, j = y-1; i >= toX && j >= toY; i--, j--){//dolava hore
                if(i != toX && j != toY && chessBoard->policka[j][i]->color != 3){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                } else if(i == toX && j == toY) {
                    if(!checkOnly){
                        return vyhod(chessBoard, piece, x, y, toY, toX);
                    }
                    return true;
                }
            }
        }
    } else if(x - toX == toY-y){
        if(toY>y){
            for(int i = x-1, j = y+1; i >= toX && j <= toY; i--, j++){//do lava dole
                if(i != toX && j != toY && chessBoard->policka[j][i]->color != 3){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                } else if(i == toX && j == toY) {
                    if(!checkOnly){
                       return vyhod(chessBoard, piece, x, y, toY, toX);
                    }
                    return true;
                }
            }
        } else {
            for(int i = x+1, j = y-1; i <= toX && j >= toY; i++, j--){//doprava hore
                if(i != toX && j != toY && chessBoard->policka[j][i]->color != 3){
                    if(!checkOnly){
                        printInvalidMove();
                    }
                    return false;
                } else if(i == toX && j == toY) {
                    if(!checkOnly){
                       return vyhod(chessBoard, piece, x, y, toY, toX);
                    }
                    return true;
                }
            }
        }
    } else {
        if(!checkOnly){
            printInvalidMove();
        }
        return false;
    }
    return false;
}

/*funkcia na vyber druhu pohybu podla typu figurky
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param x, y, toX a toY su suradnice pozicii (odkial a kam)
 @param checkOnly sluzi na potlacenie vypisov ak ide len o kontrolu moznosti pohybu
 @param ignoreColor ignoruje farbu destinacneho policka
 @return true ak bol pohyb uskutocneny  */
bool move(BOARD* chessBoard, PIECE* piece, int x, int y, int toX, int toY, bool checkOnly, bool ignoreColor){
    switch(chessBoard->policka[y][x]->type){
        case PESIAK:
            return PmovePawn(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            break;
        case STRELEC:
            return PmoveStrelec(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            break;
        case JAZDEC:
            return PmoveJazdec(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            break;
        case VEZA:
            return PmoveVeza(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            break;
        case KRALOVNA:
            if(toX == x || toY == y){
                return PmoveVeza(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            } else {
                return PmoveStrelec(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            }
            break;
        case KRAL:
            return PmoveKral(chessBoard, piece, x, y, toX, toY, checkOnly, ignoreColor);
            break;
        default:
            printf("Zadane policko neobsahuje figurku\n");
    }
    return false;
}

/*funkcia na kontrolu sachu
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param ignoreColor ignoruje farbu destinacneho policka
 @param optionalKingColor urcuje farbu krala pre prazdne policko pre ktore sa kontroluje moznost sachu
 @param ibaCekujem potlaci vypisy
 @return true ak je sach
 */
bool checkForChess(BOARD* chessBoard, PIECE* piece, bool ignoreColor, int optionalKingColor, bool ibaCekujem){
    for(int i = 0; i < NUMBER_OF_PIECES; i++){
        if(ignoreColor == true ? optionalKingColor == 0 : piece->color == 0){
            if(chessBoard->whitePieces[i]->onBoard){
                if(move(chessBoard, chessBoard->whitePieces[i], chessBoard->whitePieces[i]->yPos, chessBoard->whitePieces[i]->xPos, piece->yPos, piece->xPos, true, ignoreColor)){
                    if(!ibaCekujem){
                        printf("Cierny ma sach, figurkou %c na pozicii %c%hu\n", chessBoard->whitePieces[i]->type, 'A' + chessBoard->whitePieces[i]->xPos, chessBoard->whitePieces[i]->yPos);
                    }
                    return true;
                }
            }
        } else {
            if(chessBoard->blackPieces[i]->onBoard){
                if(move(chessBoard, chessBoard->blackPieces[i], chessBoard->blackPieces[i]->yPos, chessBoard->blackPieces[i]->xPos, piece->yPos, piece->xPos, true, ignoreColor)){
                    if(!ibaCekujem){
                        printf("Biely ma sach, figurkou %c na pozicii %c%hu\n", chessBoard->blackPieces[i]->type, 'A' + chessBoard->blackPieces[i]->xPos, chessBoard->blackPieces[i]->yPos);
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

/*funkcia kontrolujuca mat alebo pat
 @param chessBoard obsahuje smernik na sachovnicu
 @param piece je smernik na figurku
 @param matAleboPat je parameter cez ktory funkcia vracia ci ide o pat alebo mat
 @return true ak je sach mat alebo pat inak false
 */
bool checkMat(BOARD* chessBoard, PIECE* piece, bool* matAleboPat){
    //move(chessBoard, piece, piece->yPos, piece->xPos, piece->yPos+-, piece->xPos+-, true);
    bool r1 = false, r2 = false, r3 = false, r4 = false, r5 = false, r6 = false, r7 = false, r8 = false, r9 = false, r10 = false,
            r11 = false,r12 = false,r13 = false,r14 = false,r15 = false,r16 = false;
    //kontrola ak ide o prazdne policko alebo nepriatela
    if(piece->xPos != 7 ? chessBoard->policka[piece->xPos+1][piece->yPos]->color != piece->color : false){
        //ak by na tej pozicii mal sach tak true
        r1 = checkForChess(chessBoard, chessBoard->policka[piece->xPos+1][piece->yPos], true, piece->color, true);
    } else {
        //ak sa nemoze pohnut na danu poziciu tak true
        r9 = true;
    }
    if(piece->xPos != 0 ? chessBoard->policka[piece->xPos-1][piece->yPos]->color != piece->color : false){
        r2 = checkForChess(chessBoard, chessBoard->policka[piece->xPos-1][piece->yPos], true, piece->color, true);
    } else {
        r10 = true;
    }
    if(piece->yPos != 7 ? chessBoard->policka[piece->xPos][piece->yPos+1]->color != piece->color : false){
        r3 = checkForChess(chessBoard, chessBoard->policka[piece->xPos][piece->yPos+1], true, piece->color, true);
    } else {
        r11 = true;
    }
    if(piece->yPos != 0 ? chessBoard->policka[piece->xPos][piece->yPos-1]->color != piece->color : false){
        r4 = checkForChess(chessBoard, chessBoard->policka[piece->xPos][piece->yPos-1], true, piece->color, true);
    } else {
        r12 = true;
    }
    if(piece->yPos != 7 && piece->xPos != 7 ? chessBoard->policka[piece->xPos+1][piece->yPos+1]->color != piece->color : false){
        r5 = checkForChess(chessBoard, chessBoard->policka[piece->xPos+1][piece->yPos+1], true, piece->color, true);
    } else {
        r13 = true;
    }
    if(piece->yPos != 0 && piece->xPos != 0 ? chessBoard->policka[piece->xPos-1][piece->yPos-1]->color != piece->color : false){
        r6 = checkForChess(chessBoard, chessBoard->policka[piece->xPos-1][piece->yPos-1], true, piece->color, true);
    } else {
        r14 = true;
    }
    if(piece->yPos != 0 && piece->xPos != 7 ? chessBoard->policka[piece->xPos+1][piece->yPos-1]->color != piece->color : false){
        r7 = checkForChess(chessBoard, chessBoard->policka[piece->xPos+1][piece->yPos-1], true, piece->color, true);
    } else {
        r15 = true;
    }
    if(piece->yPos != 7 && piece->xPos != 0 ? chessBoard->policka[piece->xPos-1][piece->yPos+1]->color != piece->color : false){
        r8 = checkForChess(chessBoard, chessBoard->policka[piece->xPos-1][piece->yPos+1], true, piece->color, true);
    }else {
        r16 = true;
    }
    if((r1 || r9) && (r2 || r10) && (r3 || r11) && (r4 || r12) && (r5 || r13) && (r6 || r14) && (r7 || r15) && (r8 || r16)){
        if(r9 && r10 && r11 && r12 && r13 && r14 && r15 && r16){
            return false;
        }
        if(checkForChess(chessBoard, chessBoard->policka[piece->xPos][piece->yPos], true, piece->color, false)){
            *matAleboPat = false;
        } else {
            *matAleboPat = true;
        }
        return true;
    }
    return false;
}