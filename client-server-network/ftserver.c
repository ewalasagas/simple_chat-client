//Project #2 server 
//Main resources:
// OS344 -  https://oregonstate.instructure.com/courses/1729341/files/75468830/download?wrap=1
// Linux HOW-TO - http://www.linuxhowtos.org/C_C++/socket.htm
// Beej example guide/walkthrough - https://beej.us/guide/bgnet/html/multi/index.html
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <dirent.h>     // to list directory


// From OS344 - Error function used for reporting issues
void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} 


// Setup connection 
//SOURCE: http://www.linuxhowtos.org/C_C++/socket.htm
int connection_setup(int port) {
    int sockfd;

    //to ensure socket successfully created
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    // some necessary structs and info
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;    //converts into networking byte order 

    //to re-use connection
    int optval = 1;     //needed for setsockopt to get access to socket options
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

     //binds the socket to the address (current host to port number on which server is running)
    if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        error("Unable to bind!");
    }

    //Listening for 5 connections  
    if(listen(sockfd, 5) < 0){
        error("Unable to listen");
    }

    //return the sockfd
    return sockfd;
}


//Get directory contents for -l cmd and assign to list-array of pointers
//Returns int of size of directory
// SOURCE: https://www.sanfoundry.com/c-program-list-files-directory/
int returnDir(char *files[]) {
	int i = 0;
    DIR *d;
    struct dirent *dir;
    int total = 0;
    int totalSize = 0;

    d = opendir(".");
    if(d) {
    	i = 0;
        while ((dir = readdir(d)) != NULL) {
        	//Only return files of regular type - DT_REG: https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
        	if(dir->d_type == DT_REG) {
	        	files[i] = dir->d_name;
	        	totalSize += strlen(files[i]); 
	        	i++;
        	}
        }
        total = i - 1;
    }
    closedir(d);
    return total + totalSize;
} 

//Send message to client
void recv_msg(int socket, char *message, size_t size) {
	//create temp array to hold message until recieve total size then copy it into the message buffer array
	char temp[size + 1];	//for null terminator at end
	ssize_t n;
	size_t sum = 0;

	while(sum < size) {
		n = read(socket, temp + sum, size - sum);
		sum += n;
		if (n < 0){
            error("Error receiving message");
            exit(1);
        }
	}
	strncpy(message, temp, size);
}


//Recieve message to client
void send_msg(int socket, char *message) {
	ssize_t n;
	size_t size = strlen(message) + 1;
	size_t sum = 0;

	while (sum < size) {
		n = write(socket, message, size - sum);
		sum += n;
        if (n < 0) {
            error("Error sending message");
            exit(1);
        }

        else if (n == 0) {
            sum = size - sum;
        }
    }
}


//send socket number (write to client)
void sendNum(int socket, int num) {
	ssize_t n = 0;
	n = write(socket, &num, sizeof(int));
	if (n < 0) {
		printf("Error - unable to send socket number to client\n");
	}
}

//get socket number (from client)
int getNum(int socket) {
	int num;
	ssize_t n = 0;
	n = read(socket, &num, sizeof(int));
	if (n < 0) {
		printf("Error - unable to recieve socket number to client\n");
	}
	return num;
}

//return contents of file and put into buffer (w/ argument of file)
//SOURCE: https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
char* readFile(char* fileName) {
    char *source = NULL;

    FILE* fp = fopen(fileName, "r");

    if (fp == NULL) {
        error("Unable to open file");
    }

    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufSize = ftell(fp);
            if (bufSize == -1) {
                error("Invalid file");
                exit(1);
            }
            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufSize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) {
                error("Unable to read file");
            }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufSize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
    }
    fclose(fp);
    return source;
}

//Checks the argument and returns the argument number (used in main program)
int handle_request(int socket, int *data_port) {
	//This is for the recieve number
	char args[3] = "\0";

	//get the arg from the client and read it back to server
    recv_msg(socket, args, 3);

    //get the dataport and ensure return an int port # back to server is successfully read
    *data_port = getNum(socket);

	if(strcmp(args, "-l") == 0) {
		return 1;	
	}
	if(strcmp(args, "-g") == 0) {
		return 2;
	}

	return 0;
}

//Write contents of file to client
void send_file(int socket, char* file) {
	char *send;
	send = readFile(file);

	sendNum(socket, strlen(send));
	send_msg(socket, send);
}


//MAIN FUNCTION TO RUN LOOP AND CHECK/RECIEVE CMDS FROM CLIENT
int main(int argc, char *argv[]) {

	//Assign variables 
	int sockfd;
	int port;
	int newsockfd;
	int newSock;
	int dataSock;
	int dataPort;
	int pid;
	int cmd;

	if(argc < 2) {
		error("Error: please provide port number\n");
		exit(1);
	}

	//turn string input to integer
	port = atoi(argv[1]);

	//setup listening socket
	sockfd = connection_setup(port);
	printf("Connection established on port %d\n", port);

	//Continuous listening for client so connection does not close after recievein message
	while(1) {
		//Client established - new scoket created 
		newsockfd = accept(sockfd, NULL, NULL);
		//Check if error in creating scoket
		if(newsockfd < 0) {
			printf("ERROR: unable to establish new connection\n");
		}

		//create new child process
		pid = fork();
		if(pid < 0) {
			error("ERROR: unable to create child process\n");
		}

		//If fork successfully created, run the following
		if(pid == 0) {
			close(sockfd);	//close old port
			printf("Client connected started on port %d\n", port);

			cmd = handle_request(newsockfd, &dataPort);

			if(cmd == 0) {
				printf("No commands were given\nPlease use -l <DIRECTORY_NAME>, <DATA_PORT> OR -g <FILENAME>, <DATA_PORT>\n");
			}

			/**********IF -L IS RECIEVED ***********/
			if(cmd == 1) {
				//get directory name
				char *directory[100];
				int i = 0; 
				int len = returnDir(directory);

				printf("List direcotry contents on data port: %d\n", dataPort);
				
				//create new connection setup on directory
				newSock = connection_setup(dataPort);

				//Create the new socket
				dataSock = accept(newSock, NULL, NULL);
				//Check if error in creating scoket
				if(dataSock < 0) {
					printf("ERROR: unable to establish new connection\n");
				}

				//Send the socket number to client send contents of directory 
				sendNum(dataSock, len);
				while(directory[i] != NULL) {
					//write to client
					send_msg(dataSock, directory[i]);
					i++;
				}

				//end after directory sent close connection - but keep server on
				close(newSock);
				close(dataSock);
				exit(0);	
			}

			/**********IF -G IS RECIEVED ***********/
			if(cmd == 2) {
				int i = getNum(newsockfd);	//get the port number of the socket 
				char file[100] = "\0";
				recv_msg(newsockfd, file, i);
				printf("%s requested on port: %d\n", file, dataPort);

				//determine whether file can be accessed or not
				//SOURCE: https://www.ibm.com/support/knowledgecenter/en/SSB27U_6.4.0/com.ibm.zvm.v640.edclv/rtacc.htm
				if(access(file, F_OK) == -1) {
					printf("ERROR: File not found.\n");
					char error[] = "File not found";

					sendNum(newsockfd, strlen(error));
					send_msg(newsockfd, error);

					close(newSock);
					close(dataSock);
					exit(1);
				}
				else
				{
					char found[] = "File found!";
					sendNum(newsockfd, strlen(found));
					send_msg(newsockfd, found);
				}
				//print to server that file is being sent
				printf("Sending [%s] on port: %d\n", file, dataPort);

				newSock = connection_setup(dataPort);
				dataSock = accept(newSock, NULL, NULL);

				//Sending contents of file
				send_file(dataSock, file);
				close(newSock);
				close(dataSock);
				exit(0);
			}
			exit(0);
		}
	}
}