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
int login();
int sendText(int socketID);
char userName[100];

char buff[MAX_LINE];
void signio_handler(int signo)
{
	memset(buff, 0, MAX_LINE);
	recv(client_sock_fd, buff, sizeof buff, 0);
	puts(buff);
}

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		perror("lack of input parameters");
		exit(-1);
	}
	struct sockaddr_in client_socket;
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

	signal(SIGIO, signio_handler); // assign SIGIO to the handler

	//set this process to be the process owner for SIGIO signal
	if (fcntl(client_sock_fd, F_SETOWN, getpid()) < 0)
		printf("Error in setting own to socket");

	login();
	sendText(client_sock_fd);
	close(client_sock_fd);
}

int sendText(int socketID)
{
	char *text = (char *)malloc(MAX_LINE);
	while (1)
	{

		fgets(text, MAX_LINE, stdin);
		text[strlen(text) - 1] = '\0';
		if (text != NULL)
		{
			send(socketID, text, strlen(text), 0);
			if (strncmp(text, "bye", 3) == 0)
			{
				printf("Goodbye %s\n", userName);
				free(text);
				return 1;
			}
			memset(text, 0, strlen(text));
		}
		else
		{
			break;
		}
	}
	free(text);
	return 1;
}

int login()
{
	int isLogin = 0;
	char *username = (char *)malloc(100);
	char *password = (char *)malloc(100);
	while (isLogin == 0)
	{

		printf("username:");
		scanf("%s", username);
		char *sendText = (char *)malloc(MAX_LINE);
		strcat(sendText, "username:");
		strcat(sendText, username);
		send(client_sock_fd, sendText, strlen(sendText), 0);
		sleep(1);
		if (strlen(buff) > 0)
		{
			if (strncmp(buff, "Insert Password", 15) == 0)
			{
				memset(buff, 0, strlen(buff));
				while (isLogin == 0)
				{
					printf("Password:");
					scanf("%s", password);
					memset(sendText, 0, strlen(sendText));
					strcat(sendText, "password:");
					strcat(sendText, password);
					send(client_sock_fd, sendText, strlen(sendText), 0);
					sleep(1);
					if (strlen(buff) > 0)
					{
						if (strncmp(buff, "OK", 2) == 0)
						{
							isLogin = 1;
							strcpy(userName, username);
						}
						else
						{
							if (strncmp(buff, "Not OK", 6) == 0)
							{
								printf("Insert Password\n");
								memset(buff, 0, strlen(buff));
								continue;
							}
						}
					}
					break;
				}
			}
		}
	}
	memset(buff, 0, strlen(buff));
	free(username);
	free(password);
	return 1;
}