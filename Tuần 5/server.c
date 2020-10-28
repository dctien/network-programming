#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <signal.h>
#include "server.h"

int main(int argc, char const *argv[])
{
	printf("%s %d\n", "PID", getpid());
	if (argc != 2)
	{
		perror("lack of input parameters");
		exit(-1);
	}

	account *root = NULL;

	int socketIdServer;

	socketIdServer = CreateServer(AF_INET, SOCK_STREAM, 0, atoi(argv[1]));

	listen(socketIdServer, LIST_MAX);

	printf("%s\n", "Server running...waiting for connections.");
	root = readFile(root, "users.txt");

	while (1)
	{
		struct sockaddr_in client;
		socklen_t clilen = sizeof(client);
		int socketID = accept(socketIdServer, (struct sockaddr *)&client, &clilen);
		printf("%s - %d \n", "Connecting to the client", socketID);
		account *acc = login(root, socketID);
		recvMess(socketID, acc);
		close(socketID);
		writeFile(root, "users.txt");
	}
	close(socketIdServer);
	return 0;

	return 0;
}

account *readFile(account *root, char *filename)
{
	FILE *ptr = fopen(filename, "r");
	if (ptr != NULL)
	{
		char *buff = (char *)malloc(100);
		while (!feof(ptr))
		{
			fgets(buff, 100, ptr);
			root = addUser(buff, root);
			memset(buff, 0, 100);
		}
		fclose(ptr);
	}
	return root;
}
account *addUser(char *buff, account *root)
{
	account *newUser = (account *)malloc(sizeof(account));
	newUser->username = (char *)malloc(100);
	newUser->password = (char *)malloc(100);
	newUser->next = NULL;
	char *token = strtok(buff, " ");
	strcpy(newUser->username, token);
	strcpy(newUser->password, strtok(NULL, " "));
	newUser->status = atoi(strtok(NULL, " "));
	if (root == NULL)
	{
		root = newUser;
	}
	else
	{
		account *cur = root;
		while (cur->next != NULL)
		{
			cur = cur->next;
		}
		cur->next = newUser;
	}
	return root;
}

account *findAccount(account *root, char *username)
{
	account *cur = root;
	account *result = NULL;
	while (cur != NULL)
	{
		if (strcmp(cur->username, username) == 0)
		{
			result = cur;
		}
		cur = cur->next;
	}
	return result;
}
int writeFile(account *root, char *filename)
{
	FILE *ptr = fopen(filename, "w+");
	if (root != NULL && ptr != NULL)
	{
		account *cur = root;
		while (cur != NULL)
		{
			fprintf(ptr, "%s %s %d", cur->username, cur->password, cur->status);
			if (cur->next != NULL)
				fprintf(ptr, "%s", "\n");
			cur = cur->next;
		}
	}
	fclose(ptr);
}

int CreateServer(int __domain, int __type, int __protocol, int PORT)
{
	// create address server
	struct sockaddr_in server;

	int socketIdServer = socket(__domain, __type, __protocol);
	if (socketIdServer == -1)
	{
		return -1;
	}
	bzero(&server, sizeof(server));
	server.sin_family = __domain;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	bind(socketIdServer, (struct sockaddr *)&server, sizeof(server));

	return socketIdServer;
}

account *login(account *root, int socketID)
{
	account *user = NULL;
	char *buff = (char *)malloc(MAX_LINE);
	int isLogin = 0;
	while (isLogin == 0)
	{
		memset(buff, 0, strlen(buff));
		recv(socketID, buff, MAX_LINE, 0);
		puts(buff);
		if (strncmp(buff, "bye", 3) == 0)
			break;
		if (strncmp(buff, "username", 8) == 0)
		{
			account *acc = findAccount(root, buff + 9);
			if (acc != NULL)
			{
				int count = 0;
				send(socketID, "Insert Password", 17, 0);
				memset(buff, 0, strlen(buff));
				while (isLogin == 0)
				{
					recv(socketID, buff, MAX_LINE, 0);
					if (strcmp(buff + 9, acc->password) == 0)
					{
						if (acc->status == 1)
						{
							isLogin = 1;
							user = acc;
							send(socketID, "OK", 17, 0);
						}
						else
						{
							send(socketID, "Account not ready", 18, 0);
							memset(buff, 0, strlen(buff));
							break;
						}
					}
					else
					{
						memset(buff, 0, strlen(buff));
						count++;
						if (count == 3)
						{
							send(socketID, "Account is blocked", 29, 0);
							acc->status = 0;
							break;
						}
						else
							send(socketID, "Not OK", 17, 0);
						continue;
					}
				}
			}
			else
			{
				send(socketID, "Wrong account", 16, 0);
			}
			memset(buff, 0, strlen(buff));
		}
	}
	free(buff);
	return user;
}

char *handerString(char *s)
{
	char *numberlist = (char *)malloc(100);
	char *charlist = (char *)malloc(100);
	char *buff = (char *)malloc(MAX_LINE);
	int k = 0;
	int l = 0;
	for (int i = 0; i < strlen(s); i++)
	{
		if (s[i] <= '9' && s[i] >= '0')
		{
			numberlist[k++] = s[i];
		}
		else
		{
			charlist[l++] = s[i];
		}
	}
	memset(buff, 0, strlen(buff));
	numberlist[k] = '\0';
	charlist[l] = '\0';
	strcat(buff, strcat(numberlist, charlist));
	free(numberlist);
	free(charlist);
	return buff;
}

int recvMess(int socketID, account *acc)
{
	char *buff = (char *)malloc(MAX_LINE);
	char *text;
	while (recv(socketID, buff, MAX_LINE, 0) > 0)
	{
		if (strncmp(buff, "bye", 3) == 0)
		{
			memset(buff, 0, strlen(buff));
			strcat(buff, "Goodbye ");
			strcat(buff, acc->username);
			send(socketID, buff, strlen(buff), 0);
			break;
		}
		strcpy(acc->password, buff);
		text = handerString(buff);
		send(socketID, text, strlen(text), 0);
		memset(buff, 0, strlen(buff));
		memset(text, 0, strlen(text));
	}
}