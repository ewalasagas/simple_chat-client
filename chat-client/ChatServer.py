#!/usr/bin/env python
#SOURCE: https://www.youtube.com/watch?v=hFNZ6kdBgO0
import socket
import select
import sys
from thread import *

# AF_INET is the address of the domain 
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

#c heck if the arguments are there - we need address and port entered from the ones listed on client
if len(sys.argv) != 3:
	print ("Please input: script/file, IP address, port number")
	exit()

#first argument input is the IP address
IP_address = str(sys.argv[1])

#scond argument is the port number
Port = int(sys.argv[2])

#binds the server to an entered IP address and at specific PORT 
server.bind((IP_address, Port))

server.listen(5)	#listening for 100 active connections, but was not able to get to multi-clients

conn, IP_address = server.accept()
print('Connected by', IP_address)

# get your name of server-user and input
print("Enter your username: ")
name = raw_input()

# get the name of the client and input as handler?
# source: https://www.geeksforgeeks.org/python-string-split/
username = conn.recv(10).split('\0')[0]
print("You are chatting with: %s" % username)

# Sending username to client
conn.send(name)	

# source: https://stackoverflow.com/questions/34653875/python-how-to-send-data-over-tcp
while True:
	try:
		# getting the str input to send to the client
		fromServer = raw_input("%s > " % name)
		conn.send(fromServer + "\n")

		# receiving the data from client
		data = conn.recv(2048)
		# not sure if works - source: https://www.youtube.com/watch?v=hFNZ6kdBgO0
		if data.split('\0')[0] == "quit":
			print ("Connection closed..")
			conn.close()
		
		if not data: #send data?
			break

		print(username + " >" + data)

	except socket.error:
		print ("Error occurred.")
		break

conn.close()
server.close()
