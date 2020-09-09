/* Name: Elaine Alasagas
Assignment: Project 1
Class: CS372 Intro to networking
Sources: I used a variety of sources, however the base code I implemented was found through
here: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
[1] https://stackoverflow.com/questions/11743884/why-is-strcpy-also-copying-n-can-i-get-rid-of-it */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 3123
#define SA struct sockaddr

void chatSystem(int sock, char *username) {
	char serverName[10];
	char buff[MAX];
	int n;

	//This is getting the first read for the username
	read(sock, buff, sizeof(buff));
	strcpy(serverName, buff);

	int len = strlen(serverName);
	if (len > 0 && serverName[len-1] == '\n') serverName[len-1] = '\0';		//to remove new line from strcpy [1]

	bzero(buff, sizeof(buff));
	read(sock, buff, sizeof(buff));
	//The "forever" loop to indiciate turns using username given
	for(;;) {
		bzero(buff, sizeof(buff));
		printf("%s > ", username);
		n = 0;
		while ((buff[n++] = getchar()) != '\n');

		write(sock, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sock, buff, sizeof(buff));

		printf("%s > %s", serverName, buff);
		if ((strncmp(buff, "quit", 4)) == 0) {
			printf("%s has left...\n", username);
			break;
		}
	}
}

int main() {
	int sock;
	struct sockaddr_in servaddr;

	//Creating the socket using AF_INET and SOCK_STREAM per TCP
	sock = socket(AF_INET, SOCK_STREAM, 0);

	//If sock fail, then verify
	if (sock == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created...\n");

	//Using bzero - this places n zero=valued bytes in the area pointed by servaddr
	bzero(&servaddr, sizeof(servaddr));

	//assign IP, PORT
	//ensuring AF_INET is being assigned
	servaddr.sin_family = AF_INET;

	//assigning the IP to hard-coded IP address
	servaddr.sin_addr.s_addr = inet_addr("128.193.54.182"); 

	//assigning port to find (htons converts unsigned/short integer shotshort from byte order to network)
	servaddr.sin_port = htons(PORT);

	//verify the connection client socket to server socket 
	if (connect(sock, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection with the server failed ... \n");
		exit(0);
	}
	else
		printf("Connected to the server...\n");

	//Getting the username in main to pass in through chatSystem
	char username[MAX];
	printf("Enter your username: ");
	gets(username);
	write(sock, username, sizeof(username));

	//function for chatsystem
	chatSystem(sock, username);

	//close the socket onve "quit" has been entered - i.e. leave the chat-server/chat-room
	close(sock);
}