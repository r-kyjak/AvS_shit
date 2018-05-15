#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <net/if.h>
#include <errno.h>

#define ERROR (1)
#define SUCCESS (0)

#define DST_ADDR "45.33.32.156"
#define START (136)
#define END (141)

int sock;
struct sockaddr_in sockAddr;

void cleanUp() {
    close(sock);
}

int main(int argc, char* argv[]) {
    for(int i = START; i <= END; i++){
        int code = -100;
        memset(&sockAddr, 0, sizeof (sockAddr));
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket error");
            return ERROR;
        }
    
        int synRetries = 2; // Send a total of 3 SYN packets => Timeout ~7s
        if(setsockopt(sock, IPPROTO_TCP, TCP_SYNCNT, &synRetries, sizeof(synRetries)) == -1){
            perror("Error setsockopt"); //chceme pridat clenstvo v multicast skupine
            close(sock);
            return ERROR;
        }
        
        if (inet_pton(AF_INET, DST_ADDR, &sockAddr.sin_addr) <= 0) {
            perror("Error pton()");
            close(sock);
            return ERROR;
        }
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(i);
        code = connect(sock, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
        if(code == 0){
            printf("Port %d je otvoreny.\n", i);
        } else if (errno == 110){
            printf("Port %d je zatvoreny (timeout).\n", i);
        } else if (errno == 111) {
            printf("Port %d je zatvoreny (connection refused).\n", i);
        } else {
            printf("Port %d je zatvoreny (ina chyba, errno: %d, vygoogli si)\n", i, errno);
        }
        close(sock);
    }
}