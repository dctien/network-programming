#include <stdio.h>
#include <stdlib.h>
#define MAX_LINE 10240
#define LIST_MAX 8
typedef struct account
{
	char *username;
	char *password;
	int status;
	struct account *next;
} account;

int CreateServer(int __domain, int __type, int __protocol, int PORT);

account *readFile(account *root, char *filename);
int writeFile(account *root, char *filename);

account *login(account *root, int socketID);

int send_message(char *s, int socketID);
account *findAccount(account *root, char *username);
account *addUser(char *buff, account *root);
int recvMess(int socketID, account *acc);
char *handerString(char *s);