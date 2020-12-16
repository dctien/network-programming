#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXLINE 10240

int sendMessage(int socketId, char *sendline, struct sockaddr_in servaddr, char *sendTo);

int main(int *argc, char *argv[])
{
	int sockfd, n, from_len, c;
	struct sockaddr_in servaddr, from_socket;
	socklen_t addrlen = sizeof(from_socket);
	char *sendline = (char *)malloc(sizeof(MAXLINE));
	FILE *fp;

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	//inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	char *sendTo = (char *)malloc(sizeof(char) * 100);
	sendMessage(sockfd, sendline, servaddr, sendTo);
	free(sendTo);
	close(sockfd);
}

int sendMessage(int socketId, char *sendline, struct sockaddr_in servaddr, char *sendTo)
{
	printf("Enter exit to exit the conversation\n");
	struct sockaddr_in from_socket;
	socklen_t len = sizeof(from_socket);
	char *sendMess = (char *)malloc(MAXLINE);
	while (fgets(sendline, MAXLINE, stdin) != NULL)
	{
		if (strcmp(sendline, "exit") == 0)
			break;
		strcat(sendMess, sendline);
		sendto(socketId, sendMess, strlen(sendMess), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		memset(sendline, '\0', MAXLINE);
	}
}
