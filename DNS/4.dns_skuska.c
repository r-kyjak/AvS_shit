/* 
 * Pre spravny chod programu treba zakomentovat bud cast PREKLAD NA MENO alebo PREKLAD NA ADRESU
 * Program by bolo vhodne este upravit tak aby sa nemuseli komentovat casti kodu.
 * To znamena aby sa dalo zada bud MENO alebo IP ADRESA hosta a program by rozpoznal
 * co bolo zadane a podla toho by vykonal preklad z ADRESY na MENO a naopak.
 * 
 * Odporucam skusat na mene www.fri.uniza.sk resp. 158.193.138.57
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define         EXIT_ERROR          (1)
#define         LENGTH              (100)

/*
 * 
 */
int main(int argc, char** argv) {
    
    int s, v;
    struct addrinfo hints, *result, *r;
    struct sockaddr_in Adresa, ZadanaAdresa;
    // char adresaP [LENGTH];
    char hostP [LENGTH];
    
    if(argc != 2) {
        perror ("Pouzi v tvare: <program> <host>");
        exit(EXIT_ERROR);
    }
    
    
    /*
     * PREKLAD MENA NA ADRESU
     */
    
    char *meno = argv[1];
    
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    
    s = getaddrinfo(meno, NULL, &hints, &result);
    if(s != 0) {
        perror("getaddrinfo");
        exit(EXIT_ERROR);
    }
    
    struct addrinfo * I = result;
    while (I != NULL) {
        char TextAddress [INET6_ADDRSTRLEN];
        memset (TextAddress, '\0', INET6_ADDRSTRLEN);
        switch (I->ai_family) {
            case AF_INET:
                inet_ntop (I->ai_family, & ((struct sockaddr_in *) I->ai_addr)->sin_addr, TextAddress, INET6_ADDRSTRLEN);
                break;
            case AF_INET6:
                inet_ntop (I->ai_family, & ((struct sockaddr_in6 *) I->ai_addr)->sin6_addr, TextAddress, INET6_ADDRSTRLEN);
                break;
        }
        
        printf ("%s prelozene na %s\n", meno, TextAddress);
        I=I->ai_next;
    }
    freeaddrinfo(result);
    
    /*
     * PREKLAD ADRESY NA MENO
     */
    
    ZadanaAdresa.sin_family = AF_INET;
    ZadanaAdresa.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t dlz;
    dlz = sizeof(ZadanaAdresa);
    
    v = getnameinfo(&ZadanaAdresa, dlz, hostP, sizeof(hostP), NULL, 0, NI_NAMEREQD);
    if (v != 0) {
        perror("getnameinfo");
        exit(EXIT_FAILURE);
    }
    
    printf("%s prelozene na %s\n", argv[1], hostP);

    return (EXIT_SUCCESS);
}

