/* 
 * Author : Swapnil Ghonge
 * Refernces: 	https://www.geeksforgeeks.org/socket-programming-cc/
 *		https://beej.us/guide/bgnet/html/#sockaddr_inman
 *		https://beej.us/guide/bgnet/html/#sockaddr_inman
 *		https://www.csd.uoc.gr/~hy556/material/tutorials/cs556-3rd-tutorial.pdf
 *		https://man7.org/linux/man-pages/man2/bind.2.html
 *		https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mqueue.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define DEBUG (0)


struct mq_attr attr;
mqd_t mqd;

void send_data(int cli_fd)
{
	char buff[sizeof(int) + sizeof(int) + sizeof(int) + 13];
 	unsigned int priority;
	// infinite loop to send data every 2 seconds to the client from the server after sensor data over message queue
	while(1) {
	
		if(mq_receive(mqd, buff, 1024, &priority) == -1) { //Obtain sensor data over message queue
		    printf("\nERROR: mq_receive failed");
		}
   		 printf("\nserver-%s", buff);    	
		 write(cli_fd, buff, sizeof(buff)); //Send data to client

	}
}
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) 
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
	printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 		 			{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening...\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	
	printf("Accepted connection from %s", inet_ntoa(cli.sin_addr) );

  	mqd = mq_open("/sendmq", O_RDWR);  
    	
    	if(mqd == -1) {
      	  printf("\nERROR: mq_open failed");
    	}
    	
	sleep(1);
	send_data(connfd); //Send data from server to client


	close(sockfd);
}

