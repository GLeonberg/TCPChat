#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFLEN 100

int main(int argc, char* argv[])
{
	char choic[BUFLEN], nickname[BUFLEN], command[BUFLEN], ip[BUFLEN];
	bzero(&nickname, sizeof(nickname));

	// default nickname
	strcpy(nickname, "default");

	// intro menu
	system("clear");
	intro:	printf("Welcome to our chat room. Please choose an option:\n1. Choose your nickname\n2. Connect to room\n\n");
	fgets(choic, BUFLEN, stdin);
	int choice = atoi(choic);

	// nickname choice menu
	if(choice == 1)
	{
		bzero(&nickname, sizeof(nickname));
		system("clear");
		printf("Please enter a nickname: ");
		fgets(nickname, BUFLEN, stdin);
		nickname[strlen(nickname)-1] = 0;
		system("clear");
		goto intro;
	}

	system("clear");

	// get server connection info from user
	bzero(&ip, sizeof(ip));
	bzero(&choic, sizeof(choic));
	printf("Please enter the ip address of the server: ");
	fgets(ip, BUFLEN, stdin);
	ip[strlen(ip)-1] = 0;
	printf("Please enter the port number of the server: ");
	fgets(choic, BUFLEN, stdin);
	choic[strlen(choic)-1] = 0;

	printf("ip is: %s\nport is: %s\n", ip, choic);

	// launch sender process
	bzero(&command, sizeof(command));
	sprintf(command, "gnome-terminal -e \"./sender %s %s %s\"", ip, nickname, choic);
	printf("command is: %s\n", command);
	system(command);

	// launch receiver process
	bzero(&command, sizeof(command));
	sprintf(command, "gnome-terminal -e \"./receiver %s %s\"", ip, choic);
	printf("command is: %s\n", command);
	system(command);

	return 0;
}
