/* 
 * File:   main.c
 * Author: root
 *
 * Created on June 13, 2016, 5:57 PM
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
    struct addrinfo hints, *result;
    struct sockaddr_in IPAdr;
    char hostP [LENGTH];
    
    if(argc != 2) {
        perror ("Na vstupe musi byt meno / IP adresa");
        exit(EXIT_ERROR);
    }
    
    char *vstup = argv[1];
    
    if (vstup[0] >= 65)
    {
        
        /*
         * PREKLAD MENA NA ADRESU
         */

        memset(&hints, 0, sizeof (struct addrinfo));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = 0;
        hints.ai_flags = 0;

        s = getaddrinfo(vstup, NULL, &hints, &result);
        if (s != 0) {
            perror("getaddrinfo");
            exit(EXIT_ERROR);
        }

        struct addrinfo * I = result;
        while (I != NULL) {
            char Meno [INET6_ADDRSTRLEN];
            memset(Meno, '\0', INET6_ADDRSTRLEN);

            if ((I->ai_family) == AF_INET)
                inet_ntop(I->ai_family, & ((struct sockaddr_in *) I->ai_addr)->sin_addr, Meno, INET6_ADDRSTRLEN);
            if ((I->ai_family) == AF_INET6)
                inet_ntop(I->ai_family, & ((struct sockaddr_in6 *) I->ai_addr)->sin6_addr, Meno, INET6_ADDRSTRLEN);

            printf("Meno * %s * prekladam na adresu * %s *\n", vstup, Meno);
            I = I->ai_next;
        }
        freeaddrinfo(result);
    }

    else {

        /*
         * PREKLAD ADRESY NA MENO
         */

        IPAdr.sin_family = AF_INET;
        IPAdr.sin_addr.s_addr = inet_addr(argv[1]);
        socklen_t dlzka;
        dlzka = sizeof (IPAdr);

        v = getnameinfo(&IPAdr, dlzka, hostP, sizeof (hostP), NULL, 0, NI_NAMEREQD);
        if (v != 0) {
            perror("getnameinfo");
            exit(EXIT_FAILURE);
        }

        printf("Adresu * %s * prekladam na meno * %s *\n", vstup, hostP);

        return (EXIT_SUCCESS);
    }
}
