Welcome to the simple chat client-server!
Implement a client-server network application

To run the files, do the following (after files have been extracted
from zip):

1.) Run the ChatServer.py in the terminal as below on server A
"python ChatServer.py IP_ADDR_SERVER_A
NOTE: the IP address "IP_ADDR_SERVER_A and Port 3123 are hardcoded in the 
client's information

2.) Run the ChatClient.c on a different server (B or C)
NOTE: This will also run on the same server A

3.) Compile the ChatClient.c in the terminal as follows:
"gcc ChatClient.c -o client"
NOTE: you will get a warning that "gets" is deprecated - however you can 
still run the program

4). Run the  ChatClient.c file on a different server:
"./client"

5.) When the client has conected, you should be prompted to enter in 
a username 

6.) It is turned base chat, so if you type in hello on each and continue to
take turns, and hit enter - eventually you will be able to see both chat
logs on each server

7.) To quit, the client can type in "quit" to exit the program