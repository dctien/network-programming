#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_LINE 10240
int client_sock_fd;
void senText(int socketId);
int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		perror("lack of input parameters");
		exit(-1);
	}
	struct sockaddr_in server_socket;
	client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	server_socket.sin_family = AF_INET;
	server_socket.sin_port = htons(atoi(argv[2]));
	server_socket.sin_addr.s_addr = inet_addr(argv[1]);
	printf("server IP = %s ", inet_ntoa(server_socket.sin_addr));

	if (connect(client_sock_fd, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
	{
		printf("Error in connecting to server\n");
		exit(0);
	}
	else
		printf("connected to server\n");

	// Signal driven I/O mode and NONBlOCK mode so that recv will not block
	if (fcntl(client_sock_fd, F_SETFL, O_NONBLOCK | O_ASYNC))
		printf("Error in setting socket to async, nonblock mode");

	//set this process to be the process owner for SIGIO signal
	if (fcntl(client_sock_fd, F_SETOWN, getpid()) < 0)
		printf("Error in setting own to socket");
	senText(client_sock_fd);
	close(client_sock_fd);
}

void senText(int socketId){
	char *buff = (char *)malloc(1024);
	while(1){
		printf("Text:");
		fgets(buff, 1023, stdin);
		send(socketId, buff, strlen(buff), 0);
		if(strcmp(buff, "exit") == 0){
			break;
		}
		memset(buff, 0, 1024);
	}
	free(buff);
}