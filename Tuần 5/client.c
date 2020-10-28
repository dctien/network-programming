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
int checkInputString(char *s);
int sendText();
int init_single();
void termination_handler(int signum);

char userName[100];

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		perror("lack of input parameters");
		exit(-1);
	}
	init_single();
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

	login();
	sendText();
	close(client_sock_fd);
}

int login()
{
	int isLogin = 0;
	char *username = (char *)malloc(100);
	char *password = (char *)malloc(100);
	char *buff = (char *)malloc(MAX_LINE);
	while (isLogin == 0)
	{

		printf("Username:");
		scanf("%s", username);
		char *sendText = (char *)malloc(MAX_LINE);
		strcat(sendText, "username:");
		strcat(sendText, username);
		send(client_sock_fd, sendText, strlen(sendText), 0);

		if (recv(client_sock_fd, buff, MAX_LINE, 0) > 0)
		{
			if (strncmp(buff, "Insert Password", 15) == 0)
			{
				puts(buff);
				memset(buff, 0, strlen(buff));
				while (isLogin == 0)
				{
					printf("Password:");
					scanf("%s", password);
					memset(sendText, 0, strlen(sendText));
					strcat(sendText, "password:");
					strcat(sendText, password);
					send(client_sock_fd, sendText, strlen(sendText), 0);
					if (recv(client_sock_fd, buff, MAX_LINE, 0) > 0)
					{
						puts(buff);
						if (strncmp(buff, "OK", 2) == 0)
						{
							isLogin = 1;
							strcpy(userName, username);
						}
						else
						{
							if (strncmp(buff, "Not OK", 6) != 0)
							{
								memset(buff, 0, strlen(buff));
								break;
							}
							memset(buff, 0, strlen(buff));
						}
					}
				}
			}
		}
	}
	free(buff);
	free(username);
	free(password);
}

int checkInputString(char *s)
{
	int isCheck = 1;
	for (int i = 0; i < strlen(s); i++)
	{
		if (!((s[i] <= 'Z' && s[i] >= 'A') || (s[i] <= 'z' && s[i] >= 'a') || (s[i] <= '9' && s[i] >= '0')))
		{
			isCheck = 0;
			break;
		}
	}
	return isCheck;
}

int sendText()
{
	char *buff = (char *)malloc(MAX_LINE);
	char *recvline = (char *)malloc(MAX_LINE);
	while (getchar() != '\n')
		;
	while (1)
	{
		fgets(buff, MAX_LINE, stdin);
		buff[strlen(buff) - 1] = '\0';
		if (strlen(buff) != 0 && strncmp(buff, "bye", 3) != 0)
		{
			if (checkInputString(buff) == 1)
			{
				send(client_sock_fd, buff, strlen(buff), 0);
				recv(client_sock_fd, recvline, MAX_LINE, 0);
				printf("%s (from server)\n", recvline);
			}
			else
			{
				printf("Error\n");
			}

			memset(buff, 0, strlen(buff));
		}
		else
		{
			termination_handler(2);
			break;
		}
	}
	free(buff);
	free(recvline);
}

void termination_handler(int signum)
{
	char *recvline = (char *)malloc(MAX_LINE);
	send(client_sock_fd, "bye", 3, 0);
	recv(client_sock_fd, recvline, MAX_LINE, 0);
	printf("\n%s\n", recvline);
	free(recvline);
}

int init_single()
{
	struct sigaction new_action, old_action;
	new_action.sa_handler = termination_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGINT, &new_action, &old_action);  // Ctrl+C for generating the signal
	sigaction(SIGTSTP, &new_action, &old_action); // Ctrl+Z for generating the signal
	sigaction(SIGTERM, &new_action, &old_action); // kill -15 pid for generating the signal
}