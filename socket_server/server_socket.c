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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <mqueue.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define DEBUG (0)


struct mq_attr attr;
mqd_t mqd;
int sockfd, connfd;
bool signal_indication = false;

void send_data(int connfd)
{
	int bytes_sent, package_count = 1;
    	char temp_buff[20];
    	//char bme_buff[20];
    	char toClient[50];
    	unsigned int priority;
    	int temperature_data, humidity_data=0;
  
	    while(1) 
	    {
		if(mq_receive(mqd, temp_buff, sizeof(int), &priority) == -1)
		{
		    printf("\n\rError in receiving message for tmp %s", strerror(errno));
		}
		memcpy(&temperature_data, temp_buff, sizeof(int));
		
		/*if(mq_receive(mqd, bme_buff, sizeof(int), &priority) == -1)
		{
		    printf("\n\rError in receiving message for bme %s", strerror(errno));
		}
		memcpy(&humidity_data, bme_buff + sizeof(double), sizeof(double));*/
	

		sprintf(toClient, "Temperature = %d and Humidity = %d", temperature_data, humidity_data);
		
		bytes_sent = send(connfd, toClient, strlen(toClient) + 1, 0);
		if(bytes_sent == -1)
		{
		    printf("\n\rError in sending bytes.");
		    return;
		}

		package_count++;
    	    }
}
int main()
{
	int len;
    	struct sockaddr_in servaddr, cli;
    	   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) 
	{
		printf("\n\rsocket creation failed. Error: %s", strerror(errno));
		return -1;
	}
	else
	{
		printf("Socket successfully created..\n");
	} 
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) == -1)
	{
		printf("\n\rError in setting up socket options. Error: %s", strerror(errno));
		return -1;
	}
	bzero(&servaddr, sizeof(servaddr));    
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
	{
	printf("\n\rsocket bind failed. Error: %s", strerror(errno));
	exit(0);
	}
	else
	{
		printf("Socket successfully binded..\n");
	}

	if ((listen(sockfd, 5)) != 0) 
	{
		printf("\n\rListen failed. Error: %s", strerror(errno));
		exit(0);
	}
	else
	{
		printf("Server listening..\n");
		len = sizeof(cli);
	}
	mqd = mq_open("/sendmq", O_RDWR);
	if(mqd == -1)
	{
		printf("\n\rError in opening the message queue. Error: %s", strerror(errno));
	}
	while(signal_indication == false)
	{
		connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len);
		if (connfd < 0) 
		{
			printf("\n\rserver accept failed. Error: %s", strerror(errno));
			exit(0);
		}
		else
		{
	    		printf("server accept the client...\n");
		}
		send_data(connfd);
	}
	close(sockfd);
	close(connfd);
	printf("\r\n connection closed");
}
