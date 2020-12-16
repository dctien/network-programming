#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#define SERV_PORT 1255
#define MAXLINE 1024
#include "utils.h"
typedef struct client
{
	char filename[1024];
	struct sockaddr_in clientSocket;
	struct client *next;
} client;
void logFile(int socketId);
char *findAccountBySocketClient(struct sockaddr_in c);

client *root = NULL;
int main(int argc, char *argv[])
{
	int sockfd, n;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) >= 0)
	{
		printf("Server is running at port %d\n", atoi(argv[1]));
	}
	else
	{
		perror("bind failed");
		return 0;
	}

	for (;;)
	{
		logFile(sockfd);
	}
	close(sockfd);
}
void logFile(int socketIdServer){
	char *recvline = (char *)malloc(sizeof(char) * MAXLINE);
	struct sockaddr_in from_socket;
	socklen_t len = sizeof(from_socket);
	ssize_t n = recvfrom(socketIdServer, recvline, MAXLINE, 0, (struct sockaddr *)&from_socket, &len);
	puts(recvline);
	char *filename = findAccountBySocketClient(from_socket);
	FILE *ptr = fopen(filename, "a+");
	fprintf(ptr, "text:%s", recvline);
	free(recvline);
	fclose(ptr);
}

char *findAccountBySocketClient(struct sockaddr_in c)
{
	char * filename = (char *)malloc(1024);
	client *cur = root;
	client *result = NULL;
	while (cur != NULL)
	{
		if (cur->clientSocket.sin_addr.s_addr == c.sin_addr.s_addr)
		{
			result = cur;
		}
		cur = cur->next;
	}
	if(result!=NULL){
		strcpy(filename, result->filename);
	}else{
		strcat(filename, strcat(getLocalTime(), ".log"));
		cur = root;
		client * newclient = (client *)malloc(sizeof(client));
		newclient->next = NULL;
		newclient->clientSocket = c;
		strcpy(newclient->filename, filename);
		if(root==NULL) root=newclient;
		else{
			while (cur->next != NULL)
				{
					if (cur->clientSocket.sin_addr.s_addr == c.sin_addr.s_addr)
					{
						result = cur;
					}
					cur = cur->next;
				}
			cur->next = newclient;
		}
	}
	return filename;
}
