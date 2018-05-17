#include <stdio.h>
#include "moves.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 3001
#define VELKOST_IP 17
#define VELKOST_SPRAVY 7

char IP[VELKOST_IP]; // obsahuje IP adresu
char sprava[VELKOST_SPRAVY]; //obsahuje spravu
BOARD hraciePole; //sachovnica
bool gameOver = false; //urcuje koniec hry
bool tahHraca1 = true; //urcuje kto je na tahu
bool hostujem = true; //urcuje ci som hostom hry
int cisloHraca = -1; //urcuje cislo hraca
int socket_desc, spojenie; //obsahuju socket descriptory, socket_desc pouziva host na nabindovanie sa na port, 
//a klient na vytvorenie spojenia, spojenie pouziva host na prijatie spojenia s klientom
struct sockaddr_in server, client; //struktury potrebne pre sockety
    
void clear_newlines(){
    int c;
    do{
        c = getchar();
    } while (c != '\n' && c != EOF);
}

bool isValidIpAddress(char* ipAddress){
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ipAddress, &(sa.sin_addr)) != 0; //0 je nevalidna ip adresa
}

void nacitajIP(){
    do{
        printf("Zadaj IP adresu hosta:\n");
        if(fgets(IP, VELKOST_IP, stdin) != NULL){
            //clear_newlines();
            IP[strlen(IP)-1] = '\0';
        }
    } while(!isValidIpAddress(IP));  
}

/*
 * odosle vzdialenemu stroju data cez socket
 * @return true ak sa odoslanie podarilo, inak false
 */
bool posliData(char* data){
    if(hostujem){
        if(write(spojenie , data , strlen(data)) > 0){
            return true;
        }
        return false;
    } else {
        if(send(socket_desc , data , strlen(data) , 0) < 0) {
            printf("Chyba pri odosielani dat!\n");
            return false;
        }
    }
    return true;
}

/*
 * Caka na prichadzajuce data
 * @return true ak sprava prisla, false inak
 */
bool cakajNaData(){
    int velkostSpravy;
    if((velkostSpravy = recv(hostujem == true ? spojenie : socket_desc , sprava , VELKOST_SPRAVY , 0)) > 0 ) {
        return true;
    }
    return false;
}

/*
 * Nacita typ peisaka ktory si hrac vybral, po tom ako pesiakom dosiahol koniec sachovnice
 */
void precitajTypPesiaka(PIECE* p){
    printf("Vyber si typ figurky, namiesto pesiaka.\n1 Kralovna\n2 Strelec\n3 Jazdec\n4 Veza\n");
    int volba = -1;
    while(volba == -1){
        scanf("%d", &volba);
        clear_newlines();
        switch(volba){
            case 1:
                p->type = KRALOVNA;
                break;
            case 2:
                p->type = STRELEC;
                break;
            case 3:
                p->type = JAZDEC;
                break;
            case 4:
                p->type = VEZA;
                break;
            default:
                volba = -1;
                printf("Bola zadana nespravna volba!\n");
                printf("Vyber si typ figurky, namiesto pesiaka.\n1 Kralovna\n2 Strelec\n3 Jazdec\n4 Veza\n");
                break;
        }
    }
}

/*
 * Zameni pesiaka za pouzivatelom vybranu figurku
 */
void zmenPesiaka(PIECE* p){
    if(p->color == 1 && p->type == PESIAK){
        if(p->xPos == 7){
            precitajTypPesiaka(p);
        }
    } else if(p->type == PESIAK) {
        if(p->xPos == 0){
            precitajTypPesiaka(p);
        }
    }
}

/*
 * Podla toho koho je tah, bud nacita tah z klavesnice alebo caka na tah od druheho hraca
 * Kontroluje spravnost zadanych udajov, a posiela ich druhemu hracovi, pokusi sa vykonat pohyb
 */
void precitajTah(){
    char tah[VELKOST_SPRAVY];
    if((cisloHraca == 1 && !tahHraca1) || (cisloHraca == 0 && tahHraca1)){
        while(true){
            printf("Na tahu je hrac: %d\n", tahHraca1 == true ? 1 : 2);
            printf("Zadaj tah vo formate suradnic 'odkial kam' napriklad: A1 C2\n");
            if(fgets(tah, VELKOST_SPRAVY, stdin) != NULL){
                //clear_newlines();
                if(tah[0] >= 'A' && tah[0] <= 'H' && atoi(&tah[1]) >= 0 && atoi(&tah[1]) <= 7 && tah[2] == ' ' && tah[3] >= 'A' && tah[3] <= 'H' && atoi(&tah[4]) >= 0 && atoi(&tah[4]) <= 7){
                    if(tahHraca1 == true ? hraciePole.policka[tah[0]-65][atoi(&tah[1])]->color == 1 : hraciePole.policka[tah[0]-65][atoi(&tah[1])]->color == 0){
                        if(move(&hraciePole, hraciePole.policka[tah[0]-65][atoi(&tah[1])], atoi(&tah[1]), tah[0]-65, atoi(&tah[4]), tah[3]-65, false, false)){
                            zmenPesiaka(hraciePole.policka[tah[3]-65][atoi(&tah[4])]);
                            tahHraca1 = !tahHraca1;
                            int i = 10;
                            tah[5] = hraciePole.policka[tah[3]-65][atoi(&tah[4])]->type;
                            tah[6] = '\0';
                            while(!posliData(tah) && i-- != 0){
                            
                            }
                            if(i == 0){
                                gameOver = true;
                                printf("Chyba spojenia, ukoncujem hru!\n");
                            }
                            break;
                        } else {
                            printf("Nepovoleny pohyb! (Hybes sa do sachu?)\n");
                        }
                    } else {
                        printf("Musis hybat svojou figurkou!\n");
                    }
                } else {
                    printf("Nespravne zadane suradnice!\n");
                }
            }
        } 
    } else {
        if(cakajNaData()){
            if(move(&hraciePole, hraciePole.policka[sprava[0]-65][atoi(&sprava[1])], atoi(&sprava[1]), sprava[0]-65, atoi(&sprava[4]), sprava[3]-65, false, false)){
                tahHraca1 = !tahHraca1;
                hraciePole.policka[sprava[3]-65][atoi(&sprava[4])]->type = sprava[5];
            } else {
                gameOver = true;
                printf("Klient zaslal zly tah, ukoncujem hru, vyhral si kontumacne!\n");
            }
        } else {
            printf("Klient nezaslal svoj tah, ukoncujem hru, vyhral si kontumacne!\n");
            gameOver = true;
        }
    }

}

/*
 * Inicializuje sachovnicu a vypise ju
 */
void setUpGame(){
    //
    initBoard(&hraciePole);
    printBoard(&hraciePole);
}

/*
 * Uvolni pamat
 */
void cleanUP(){
    close(socket_desc);
    if(hostujem){
        close(spojenie);
    }
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            free(hraciePole.policka[i][j]);
            hraciePole.policka[i][j] = NULL;
        }
    }
    
    for(int i = 0; i < lostBlackPieces.size; i++){
        free(lostBlackPieces.pieces[i]);
        lostBlackPieces.pieces[i] = NULL;
    }
    for(int i = 0; i < lostWhitePieces.size; i++){
        free(lostWhitePieces.pieces[i]);
        lostWhitePieces.pieces[i] = NULL;
    }
}

/*
 * Obsahuje game loop
 * nacitava a vykonava jednotlive tahy, kontroluje sach mat
 */
void vytvorHru(){
    bool sachMatAleboPat;
    while(!gameOver){
        precitajTah();
        printBoard(&hraciePole);
        if(checkMat(&hraciePole, tahHraca1 == true ? hraciePole.whiteKing : hraciePole.blackKing, &sachMatAleboPat)){
            if(!sachMatAleboPat){
                tahHraca1 == true ? printf("Sach mat, hrac 2 vyhrava!\n") : printf("Sach mat, hrac 1 vyhrava!\n");
            } else{
                printf("Pat\n");
            }
            gameOver = true;
        }
    }
    //zatvor sockety
    cleanUP();
}

/*
 * Nabinduje socket na port, a caka na spojenie, ktore potom otvori
 */
bool cakajNaSpojenie(){
    int c;
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("Nepodarilo sa vytvorit socket!\n");
        return false;
    }
    //Priprav socaddr sturkturu
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    //Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        printf("Chyba pri bindovani socketu!\n");
        return false;
    }
    //Pocuvaj
    listen(socket_desc , 3);
    //Prijatie spojenia
    printf("Cakam na spojenie...\n");
    c = sizeof(struct sockaddr_in);
    spojenie = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (spojenie<0) {
        printf("Chyba pri prijati spojenia!\n");
        return false;
    }
    printf("Spojenie prebehlo uspesne, klient je pripojeny!\n");
    return true;
}

/*
 * Pripoji sa na hosta pomocou socketov
 */
bool pripojSa(){
    //Vytvor socket
    nacitajIP();
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("Nepodarilo sa vytvorit socket!\n");
        perror("socket");
        return false;
    }
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
 
    //Pripoj sa na vzdialeneho hosta
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0){
        printf("Chyba pri pripojeni!\n");
        perror("socket");
        return false;
    }
    printf("Pripojenie prebehlo uspesne!\n");
    hostujem = false;
    return true;
}

/*
 * Spravuje hlavne menu programu
 */
void mainMenu(){
    int volba = -3;
    while(volba != 0){
        printf("Multiplayer sach\n\n0 pre koniec\n1 pre hostovanie hry\n2 pre pripojenie k hre\n\nZadajte volbu:\n");
        scanf("%d", &volba);
        clear_newlines();
        switch(volba){
            case 1:
                cisloHraca = 0;
                if(cakajNaSpojenie()){
                    setUpGame();
                    vytvorHru();
                } 
                volba = -3;
                break;
            case 2:
                cisloHraca = 1;
                if(pripojSa()){
                    setUpGame();
                    vytvorHru();
                } 
                volba = -3;
                break;
            case 0:
                printf("Koniec");
                break;
            default:
                printf("Zadana nespravna volba!\n");
                volba++;
                break;
        }
    }
}

int main(){
    mainMenu();
}