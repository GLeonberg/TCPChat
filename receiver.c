// required libraries
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFLEN 100

int main(int argc, char* argv[])
{
	char buf[BUFLEN];
	bzero(&buf, sizeof(buf));

	struct sockaddr_in serveraddr;
	int clientfd = 0;
	char* ip = argv[1];
	int port = atoi(argv[2]);

	// connect to server
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("Error Creating Socket!");

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = inet_addr(ip);

	if (connect(clientfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0)
		perror("Could not connect!");

	write(clientfd, " ", 1);
	
	// infinite loop of get messages from server, print message
	while(1)
	{
		read(clientfd, buf, 100);
		printf("%s\n", buf);
		bzero(&buf, sizeof(buf));
	}

	return 0;
}
