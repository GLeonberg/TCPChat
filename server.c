#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>


#define PROGNAME "./server"
#define MAXCLIENTS 100
#define BUFLEN 100

static const char RED[] = "\033[31m";
static const char GRN[] = "\033[32m";
static const char YEL[] = "\033[33m";
static const char BLU[] = "\033[34m";
static const char MAG[] = "\033[35m";
static const char RST[] = "\033[0m";

void* handleClient(void* param);

// shared variables for all threads
static pthread_mutex_t lck;
static FILE* log;
static int* fds;
static int i = 0;

void err()
{
	perror(PROGNAME);
	exit(1);
}

void blank()
{
	return;
}

int main(int argc, char* argv[])
{
	signal(SIGPIPE, blank);
	fds = malloc(sizeof(int)*MAXCLIENTS);

	// create log file
	if ( (log = fopen("log.txt", "w")) == NULL )
		err();

	// make socket
	int fd = 0;
	if( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err();

	// bind
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if( bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		err();

	// listen
	if( listen(fd, 5) < 0)
		err();

	// infinite loop of accepting and processing clients
	while(1)
	{
		// accept request, store client info
		int cfd = 0;

		struct sockaddr_in client_addr;
		int client_size = sizeof(client_addr);
		
		printf("Waiting on a new client on port %d\n\n", atoi(argv[1]));
		
		if( (cfd = accept(fd, (struct sockaddr*) &client_addr, &client_size)) < 0)
			err();

		printf("A new connection has occured!\n\n");

		fds[i++] = cfd;

		// create thread to handle new client
		pthread_t thread;
		pthread_create(&thread, NULL, handleClient, (void*) cfd);
		pthread_detach(thread);
	}
	

	return 0;
}

void* handleClient(void* param)
{
	// ignore broken pipes
	signal(SIGPIPE, blank);
	
	char nickname[BUFLEN];
	bzero(&nickname, sizeof(nickname));
	int j = 0;
	char mssg[BUFLEN];
	bzero(&mssg, sizeof(mssg));
	int cfd = (int)param;
	char intro1[] = "User ";
	char intro2[] = " has joined the room";
	
	read(cfd, mssg, sizeof(mssg));
	pthread_mutex_lock(&lck);
	
	strcpy(nickname, mssg);
	
	if(strlen(nickname) > 2)
	{
		for(j = 0; j < i; j++)
		{
			char temp[strlen(MAG) + strlen(intro1) + strlen(nickname) + strlen(intro2) + strlen(RST)+1];
		
			bzero(&temp, sizeof(temp));
			strcpy(temp, MAG);
			strcpy(temp+strlen(temp), intro1);
			strcpy(temp+strlen(temp), nickname);
			strcpy(temp+strlen(temp), intro2);
			strcpy(temp+strlen(temp), RST);
			write(fds[j], temp, sizeof(temp));
		}
	
		fprintf(log, "%s%s%s%s%s\n", MAG, intro1, nickname, intro2, RST);
	}
	
	pthread_mutex_unlock(&lck);
	
	while(1)
	{
		signal(SIGPIPE, blank);
		int private = 0;
		int color = 0;
		bzero(&mssg, sizeof(mssg));

		// read message
		read(cfd, mssg, sizeof(mssg));
		
		// handle user leaving the room
		if(strlen(mssg) == 0)
		{
			if(strlen(nickname) > 2)
			{
				bzero(&mssg, sizeof(mssg));
				strcpy(mssg, MAG);
				strcpy(mssg+strlen(mssg), "User ");
				strcpy(mssg+strlen(mssg), nickname);
				strcpy(mssg+strlen(mssg), " has left the room");
				strcpy(mssg+strlen(mssg), RST);
			
				for(j = 0; j < i; j++)
				{
					signal(SIGPIPE, blank);
					printf("file descriptor is: %d\n", fds[j]);
					write(fds[j], mssg, sizeof(mssg));
				}
			}
			
			return 0; // handle client exit
		}
		
		pthread_mutex_lock(&lck);
		
		// parse message for arguments
		char* temp2 = malloc(strlen(mssg));
		strcpy(temp2, mssg);
		strsep(&temp2, " ");
		char* arg1 = strsep(&temp2, " ");
		char* flag1 = strsep(&arg1, "=");
		
		if( strcmp(flag1, "color") == 0) color = 1;
		
		
		char mssg2[strlen(mssg)+20];
		bzero(&mssg2, sizeof(mssg2));
		
		// handle color argument
		if(color == 1)
		{
			strcpy(mssg2, nickname);
			strcpy(mssg2+strlen(mssg2), ": "); 
			
			if(strcmp(arg1, "red") == 0)
				strcpy(mssg2+strlen(mssg2), RED);
			else if(strcmp(arg1, "green") == 0)
				strcpy(mssg2+strlen(mssg2), GRN);
			else if (strcmp(arg1, "yellow") == 0)
				strcpy(mssg2+strlen(mssg2), YEL);
			else
				strcpy(mssg2+strlen(mssg2), BLU);
			
			printf("%s\n%s\n", flag1, arg1);
			printf("%s\n", mssg);
			printf("%s\n", mssg+strlen(arg1)+1+strlen(flag1)+1);
			
			strcpy(mssg2+strlen(mssg2), mssg+strlen(nickname)+1+strlen(arg1)+1+strlen(flag1)+2);	
			strcpy(mssg2+strlen(mssg2), RST);
		}
		
			if(color == 0)
			{
				// echo message to all connected clients
				for(j = 0; j < i; j++)
				{
					printf("file descriptor is: %d\n", fds[j]);
					write(fds[j], mssg, sizeof(mssg));
				}
			
				// log message
				fprintf(log, "%s\n", mssg);
				fflush(log);
				pthread_mutex_unlock(&lck);
			}
			else
			{
				// echo message to all connected clients
				for(j = 0; j < i; j++)
				{
					printf("file descriptor is: %d\n", fds[j]);
					write(fds[j], mssg2, sizeof(mssg2));
				}
			
				// log message
				fprintf(log, "%s\n", mssg2);
				fflush(log);
				pthread_mutex_unlock(&lck);
			}

	}

	exit(0);
}
