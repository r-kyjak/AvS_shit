#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <net/if.h>

#define SUCCESS (1)
#define ERROR (0)

#define MSIZE (1472) //1518 - 18 - 20 - 8
#define PORT (9999)
#define NICKNAME_SIZE (10)
#define SKUPINY_SIZE (89)
#define DEF_GRP "10"
#define IF_NAME "eth0"
#define DST_ADDR "239.0.0.1"

char nickname[NICKNAME_SIZE];
char nicknameSend[NICKNAME_SIZE + 2];
char sprava[MSIZE + NICKNAME_SIZE + 2];
int sock;
struct sockaddr_in sockAddr;
int prihlaseny = 0; 
char message[MSIZE + NICKNAME_SIZE + 2];
char prevSkupina[3] = DEF_GRP;
int size;
_Bool poleSkupin[SKUPINY_SIZE];
char currSkupina[3] = DEF_GRP;


void flushPoleSkupin();
void menu();
int skontrolujLogin();

void *receiver(void *arg) {        
	int sock = *((int*)arg);
	struct sockaddr_in sockAddr;
	char help[3];
	int moj;
	char check[3];
	check[2]='\0';
	for(;;) {
                 
		memset(message, '\0', sizeof(message));
		size = sizeof(sockAddr);
		if(recvfrom(sock, message, MSIZE, 0, (struct sockaddr *)&sockAddr, &size) == -1) {
			perror("Error recvfrom()");
			break;
		}
                moj = 0;
                
                if(prihlaseny == 1) {
                    if((strcmp(message,strcat(strcat(strcpy(check,currSkupina),"> Pripojil sa> "),nickname)) == 0)){
                        moj = 1;
                    }else if ((strncmp(message,strcat(strcat(strcat(strcpy(check,currSkupina),"> "),nickname),">"),strlen(currSkupina)+3+strlen(nickname)) == 0)){
                        moj = 1;
                    }else if ((strcmp(message,strcat(strcat(strcpy(check,prevSkupina),"> Odpojil sa> "),nickname)) == 0)){
                        moj = 1;
                    }
                    
                    strncpy(help,message,2);
                    help[2]='\0';
                    if(moj == 0 && poleSkupin[atoi(help)-10]==1){
                        printf("[%s] \n", message);
                    }
                }
		
		}
                
        }

int login(){
    do{
        printf("Zadaj nickname\n");
        fgets(nickname,NICKNAME_SIZE,stdin);
    } while (strlen(nickname) == 1); 
    
    flushPoleSkupin();
    strcpy(currSkupina,DEF_GRP);
    
    nickname[strlen(nickname) - 1] = '\0';
    strcpy(sprava, currSkupina);
    strcat(sprava, "> ");
    strcat(sprava, "Pripojil sa> ");
    strcat(sprava, nickname);
    
    if(sendto(sock, sprava, strlen(sprava), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
	perror("Error sendto()");
    }
    prihlaseny = 1;
    return 1;
}

void logout(){
    strcpy(sprava, DEF_GRP);
    strcat(sprava, "> Odpojil sa> ");
    strcat(sprava, nickname);
    
    if(sendto(sock, sprava, strlen(sprava), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
	perror("Error sendto()");
    }
    
    prihlaseny = 0;
}

int skontrolujPrikaz(char *message){
    char help[3];
    
    if(strcmp(message,"/login") == 0){        
        if(skontrolujLogin() == 1){
            printf("Uz si lognuty, najprv sa logoutni\n");
        }        
        return 1;
    } else if(strcmp(message,"/logout") == 0){        
        logout();        
        return 1;
    } else if(strcmp(message,"/exit") == 0){
        logout();
        close(sock);
        exit(EXIT_SUCCESS);
        return 1;
    } else if(strncmp(message,"/leave",6) == 0){
        if(message[7] == '1' && message[8] == '0'){
            printf("Defaultnu skupinu nemozno leavnut\n");
            return 1;
        }
        strncpy(help,&message[7],2);
        help[2]='\0';        
        if(poleSkupin[atoi(help)-10]==0){
            printf("Error, do danej skupiny nepatris\n");
            return 1;
        }
        printf("Odchadzas do zo skupiny\n");
        strcpy(prevSkupina,currSkupina);
        strncpy(currSkupina,&message[7],2);
        poleSkupin[atoi(currSkupina)-10] = 0;
        strcpy(sprava,currSkupina);
        strcat(sprava, "> Odpojil sa> ");
        strcat(sprava, nickname);
        strcpy(currSkupina,DEF_GRP);
        
        if(sendto(sock, sprava, strlen(sprava), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
            perror("Error sendto()");
        }
        return 1;
    } else if(strncmp(message,"/join",5) == 0){
        strncpy(help,&message[6],2);
        help[2]='\0';
        if(poleSkupin[atoi(help)-10]==1){
            printf("Prepol si sa do skupiny\n");
            strncpy(currSkupina,&message[6],2);
            return 1;
        }
        printf("Pridal si sa do skupiny\n");
        strncpy(currSkupina,&message[6],2);
        poleSkupin[atoi(currSkupina)-10] = 1;
        strcpy(sprava,currSkupina);
        strcat(sprava, "> Pripojil sa> ");
        strcat(sprava, nickname);
    
        if(sendto(sock, sprava, strlen(sprava), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
            perror("Error sendto()");
        }
        return 1;
    } else if(strcmp(message,"/menu") == 0){
        menu();
        return 1;
    } else if(strcmp(message,"/vypis") == 0){
        printf("10,");
        for(int i=1;i<SKUPINY_SIZE;i++){
            if(poleSkupin[i] == 1){
                printf("%i,",i+10);
            }
        }
        printf("\n");
        printf("Aktualna skupina: %s\n",currSkupina);
        return 1;
    }
    return 0;
}

int skontrolujLogin(){
    if(prihlaseny == 0){
        return 0;
    } else {
        return 1;
    }
}

void menu(){
    printf("--------------------------------------\n");
    printf("------------Vitajte v chate-----------\n");
    printf("1. Pri vstupe treba zadat meno(2 s rovnakym menom nie su osetreni, nerobit :D)\n");
    printf("2. Kazdy uzivatel je defaultne po logine v skupine 10\n");
    printf("3. Chat prikazy idu len pre lognuteho uzivatela\n");
    printf("4. /logout na logoutnutie\n");
    printf("5. /join xx(cislo 11-98) na joinutie inej skupiny(zadanie skupiny mimo rozsah nie je osetrene)\n");
	printf("6. join sluzi aj na prepinanie (prepinanie medzi skupinami do kt. patrim aby som do danej skupiny zasielal spravy)\n");
    printf("7. /leave xx(cislo 11-98) leavne skupinu(zadanie skupiny mimo rozsah nie je osetrene)\n");
    printf("8. /exit na vypnutie programu\n");
    printf("9. /menu na vypis tohoto menu\n");
    printf("10. /vypis na zoznam skupin v ktorych som + aktivnu\n");
    printf("11. system neustale nepyta input, no pocuva ;)\n");
    printf("12. pises len do jednej skupiny (default 10 alebo ina ak /join xx)\n");
    printf("13. pocuvas vo vsetkych skupinach v ktorych si\n");
    printf("14. kompiluj ma: gcc main.c -o chat -lpthread\n");
    printf("--------------------------------------\n");
}

void flushPoleSkupin(){
    for(int i =0;i<SKUPINY_SIZE;i++){
            poleSkupin[i] = 0;
        }
        poleSkupin[0] = 1;       
}

int main(int argc, char* argv[]) {
    
        flushPoleSkupin();
        char message[MSIZE];

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("Socket error");
		return ERROR;
	}

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(PORT);

         
	const char device[] = IF_NAME;

	if(setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, device, sizeof(device)) == -1) {
		perror("Error setsockopt_bind");
		close(sock);
		return ERROR;
	}


    	if(inet_pton(AF_INET, DST_ADDR, &sockAddr.sin_addr) <= 0) {
		perror("Error pton()");
		close(sock);
		return ERROR;
	}


	struct ip_mreqn multiJoin;

	if(inet_pton(AF_INET, DST_ADDR, &multiJoin.imr_multiaddr) <= 0) {
		perror("Error pton()_multi");
		close(sock);
		return ERROR;
	}
        
                
        int ifIndex;
	if((ifIndex = if_nametoindex(IF_NAME)) == 0) {
		perror("Error if_nametoindex()");
		close(sock);
		return ERROR;
	}
        
	multiJoin.imr_address.s_addr = INADDR_ANY;
        multiJoin.imr_ifindex = ifIndex;

	if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multiJoin, sizeof(multiJoin)) == -1) {
		perror("Error setsockopt_multicast"); //chceme pridat clenstvo v multicast skupine
		close(sock);
		return ERROR;
	}
        
        int enable = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
            perror("setsockopt(SO_REUSEADDR) failed");

	if(bind(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
		perror("Error pton()");
		close(sock);
		return ERROR;
	}
        menu();
	pthread_t receiveThread;
	pthread_create(&receiveThread, NULL, receiver, (void *)&sock);
        
        login();
        
        strcpy(nicknameSend,nickname);
        strcat(nicknameSend,"> ");
	for(;;) {
                        if(skontrolujLogin() == 0){
                            login();
                            strcpy(nicknameSend,nickname);
                            strcat(nicknameSend,"> ");
                        }
			memset(message, '\0', sizeof(message));
                        memset(sprava, '\0', sizeof(sprava));
			fgets(message, MSIZE, stdin);
                        message[strlen(message) - 1]='\0';                        
                        if(skontrolujPrikaz(message) == 0){
                            strcpy(sprava,currSkupina);
                            strcat(sprava,"> ");
                            strcat(sprava, nicknameSend);
                            strcat(sprava, message);
                            if(sendto(sock, sprava, strlen(sprava), 0, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1) {
				perror("Error sendto()");
				break;
                            }
                        }
		}
}


