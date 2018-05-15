/* 
 * File:   main.c
 * Author: student
 *
 * Created on May 30, 2016, 2:16 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8888
#define MSGLEN 2000
#define ASCIILEN ((126-32)+1)
/*
 * 
 */
char ascii[ASCIILEN];

void * client_thread (void * arg) {
    int Socket= *((int *) arg);
    ssize_t w;
    for(;;) {
        w=write (Socket, ascii, strlen(ascii));
        if(w<strlen(ascii)) {
            close(Socket);
            free(arg);
            pthread_exit(NULL);
        }
    }
}
int main(int argc, char** argv) {
    int Socket, client_Socket, i, c;
    struct sockaddr_in server, client;
    char client_message[MSGLEN], server_reply[MSGLEN];
    
    for(i=32; i<127; i++)
    {        
        ascii[i-32]=i;
    }
    
    Socket = socket(AF_INET, SOCK_STREAM, 0);
    
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=INADDR_ANY;
    
    bind(Socket, (struct sockaddr *) &server, sizeof(server));
    
    listen(Socket, 3);
    
    for(;;) {
        int * client_Socket_copy;
        pthread_t daco;
        
        c=sizeof(struct sockaddr_in);
        client_Socket=accept(Socket, (struct sockaddr *) &client, (socklen_t *) &(c));
    
        client_Socket_copy = malloc(sizeof(int));
        
        *client_Socket_copy=client_Socket;
        
        pthread_create(&daco, NULL, client_thread, client_Socket_copy);
    } 
        
    //close(Socket);
    return (EXIT_SUCCESS);
}

