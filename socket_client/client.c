/*
 *Author: Sricharn Kidambi
 *Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(int sockfd)
{
    char buff[42];
    int rv;
    while(1) 
    {
        while((rv = recv(sockfd, buff, sizeof(buff), 0)) != 41);
        if(rv == -1)
        {
            printf("\n\rError: %s", strerror(errno));
            return;
        }
        printf("\n\r%s", buff);
        printf("\n\r%d bytes were received", rv);
        if ((strncmp(buff, "exit", 4)) == 0) 
        {
            printf("Client Exit...\n");
            break;
        }
    }
}
   
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    char IP[20];
    
    if(argc != 2)
    {
        printf("\n\rEnter with IP Address of the server");
        return -1;
    }
    printf("\n\rConnecting to the server.");
    strcpy(IP, argv[1]);
    printf("\n\rContents of IP array = %s", IP);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) 
    {
        printf("\n\rSocket creation failed %s", strerror(errno));
        return -1;
    }
    else
    {
        printf("\n\rSocket creation successful!!");
    }
    bzero(&servaddr, sizeof(servaddr));
   
   
    servaddr.sin_family = AF_INET;
    
    servaddr.sin_addr.s_addr = inet_addr(IP); 
    servaddr.sin_port = htons(PORT);
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) 
    {
        printf("\n\rconnection with the server failed. Error: %s", strerror(errno));
        close(sockfd);
        return -1;
    }
    else
    {
        printf("\n\rconnected to the server.");
    }
   
    
    func(sockfd);
    close(sockfd);
}
