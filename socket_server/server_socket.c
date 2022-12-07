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

void graceful_exit()
{
    int bytes_sent = send(connfd, "exit", strlen("exit") + 1, 0);
    if(bytes_sent == -1)
    {
        printf("\n\rTermination signal to the client could not be sent. Error: %s", strerror(errno));
    }
    if(sockfd > -1)
    {
        int rv = shutdown(sockfd, SHUT_RDWR);
        if(rv)
        {
            printf("\n\rError in shutdown. Error: %s", strerror(errno));
        }
    }
}
static void signal_handler(int signal)
{
    switch(signal)
    {
        case SIGINT:
          printf("\n\rSIGINT Signal Detected.");
          break;
          
        case SIGTERM:
          printf("\n\rSIGTERM Signal Detected.");
          break;
          
        default:
          printf("\n\rSome Signal Detected.");
          break;
     }
    signal_indication = true;
    graceful_exit();
}
void func(int connfd)
{
    int bytes_sent, package_count = 1;
    char buff[1024];
    char toClient[50];
    unsigned int priority;
    float temperature_data;
    // infinite loop for chat
    while(1) 
    {
	if(mq_receive(mqd, buff, sizeof(float), &priority) == -1)
	{
	    printf("\n\rError in receiving message from the queue. Error: %s", strerror(errno));
	}
	memcpy(&temperature_data, buff, sizeof(float));
	
	sprintf(toClient, "Temperature = %0.2lf", temperature_data);
	if(signal_indication)
	{
	    break;
	}
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
    
    sig_t rs = signal(SIGINT, signal_handler);
    if (rs == SIG_ERR) 
    {
        printf("\n\rError in registering SIG_ERR.");
        graceful_exit();
        return -1;
    }

    rs = signal(SIGTERM, signal_handler);
    if (rs == SIG_ERR) 
    {
        printf("\n\rError in registering SIGTERM.");
        graceful_exit();
        return -1;
    }
    
    // socket create and verification
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
        graceful_exit(); 
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("\n\rsocket bind failed. Error: %s", strerror(errno));
	exit(0);
    }
    else
    {
	printf("Socket successfully binded..\n");
    }
    // Now server is ready to listen and verification
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
    // Accept the data packet from client and verification
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
    // Function for chatting between client and server
        func(connfd);
    }
    // After chatting close the socket    
    close(sockfd);
    close(connfd);
    printf("\n\rConnection closed...");
    return 0;
}
