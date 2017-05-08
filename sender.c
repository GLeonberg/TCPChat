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
#define lineClear() printf("\33[2K\r")
#define goUp(x) printf("\033[%dA", (x))

int main(int argc, char* argv[])
{
	char buf[BUFLEN];
	struct sockaddr_in serveraddr;
	int clientfd = 0;
	char* ip = argv[1];
	char* nickname = argv[2];
	int port = atoi(argv[3]);
	char* mssg = malloc(BUFLEN);
	int k = 0;

	for(k = 0; k < strlen(nickname); k++)
		buf[k] = nickname[k];
		

	//printf("ip is: %s\nport is: %d\n", ip, port);

	// connect to server
	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("Error Creating Socket!");

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = inet_addr(ip);

	if (connect(clientfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0)
		perror("Could not connect!");

	//printf("socket fd is: %d\n", clientfd);
	
	// send nickname to server for intro
	write(clientfd, nickname, strlen(nickname));

	// infinite loop of getting input and sending to server
	while(1)
	{
		bzero(&buf, BUFLEN);

		// append nickname to front of buffer
		for(k = 0; k < strlen(nickname); k++)
			buf[k] = nickname[k];
		buf[k++] = ':';
		buf[k] = ' ';

		// get message fom user
		goUp(1);
		lineClear();
		printf("> ");
		fgets(buf+strlen(nickname)+2, BUFLEN-strlen(nickname)-2, stdin);
		buf[strlen(buf)-1] = 0;

		// send mssg over socket to server
		write(clientfd, buf, sizeof(buf));
	}

	free(mssg);
	return 0;
}
