/*
 *Author: Sricharn Kidambi
 *Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define IP_ADDRESS_SIZE 20
char datafromserver[1000];
   
int main(int argc, char *argv[])
{
   int socketfd = 0;
	int socketconnectfd = 0;
	openlog("Socket Application client",LOG_PID,LOG_USER);
	printf("Welcome to Socket Application - CLIENT\n");
	char server_ipaddress[IP_ADDRESS_SIZE] = {0};
	memcpy(server_ipaddress, argv[1], strlen(argv[1]));
	/*Socket address structure values*/
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	syslog(LOG_DEBUG, "Client started");
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)
	{
		syslog(LOG_DEBUG, "ERROR: In creating Socket file descriptor");
		exit(0);
	}

	if((inet_pton(AF_INET, server_ipaddress, &server_address.sin_addr)) < 0)
	{
		syslog(LOG_DEBUG, "ERROR: In converting IPV4 from text to binary form");
		exit(0);
	}
	
	socketconnectfd =  connect(socketfd, (struct sockaddr *)&server_address,sizeof(server_address));
	if(socketconnectfd < 0)
	{
		syslog(LOG_DEBUG, "ERROR: In connecting to Server");
		exit(0);
	}
	
	syslog(LOG_DEBUG, "connected to Server");
	printf("connected to Server\n");
	while (1)
	{
	       read(socketfd,datafromserver,sizeof(datafromserver));
	       

	}
	return 0;
}

