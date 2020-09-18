Compile file-transfer client/server 
Implement 2-connection client-server network application

## How to run
1.) To compile the the server file, type in "gcc ftserver.c -o server"

2.) Run "./server PORT_NUM" -- PORT_NUM I have been using is 8123 

3.) To compile the client file, type in "python3 ftclient.py [SERVER_IP] [SERVER_PORT_NUM] [-l OR -g CMD] [DATA_PORT_NUM]
EXAMPLE:
	server running on SERVER1:	./server 8123
	client running on SERVER2:	python3 ftclient.py SERVER1_IP_ADDR -l CS372 50000

NOTES: For the DATA_PORT numbers, I've been using 50000 or 50001 (as long as it's under 65535)
