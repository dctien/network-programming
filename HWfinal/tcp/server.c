#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "utils.h"

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/
void logFile(struct sockaddr_in client, int socketId);
int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 pid_t childpid;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;

 //Create a socket for the soclet
 //If sockfd<0 there was an error in the creation of the socket
 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }


 //preparation of the socket address
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 servaddr.sin_port = htons(atoi(argv[1]));

 //bind the socket
 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, LISTENQ);

 printf("%s\n","Server running...waiting for connections.");

 for ( ; ; ) {

  clilen = sizeof(cliaddr);
  //accept a connection
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

  printf("%s\n","Received request...");

  if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

    printf ("%s\n","Child created for dealing with client requests");

    //close listening socket
    close (listenfd);
	logFile(cliaddr, connfd);
    exit(0);
  }
 //close socket of the server
 close(connfd);
 
 }
}

void logFile(struct sockaddr_in client, int socketId){
	printf("%d", socketId);
	char *filename = (char *)malloc(1024);
	strcat(filename, strcat(getLocalTime(), ".log"));
	FILE *ptr = fopen(filename, "w+");
	fprintf(ptr,"client.sin_port:%d\nclient.sin_addr.s_addr:%d\nclient.sin_family:%d\nclient.sin_zero:%s\n", client.sin_port, client.sin_addr.s_addr, client.sin_family, client.sin_zero);
	char *buff = (char *)malloc(1024);
	while(1){
		ssize_t n = recv(socketId, buff, 1024, 0);
		if(n > 0){
			fprintf(ptr, "text:%s", buff);
		} else {
			break;
		}
		memset(buff, 0, 1024);
	}
	fclose(ptr);
}
