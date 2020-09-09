#!/usr/bin/env python3
# Used a number of resources:
# SOURCE: https://www.youtube.com/watch?v=hFNZ6kdBgO0
# Python program to implement client side of chat room. 
import socket 
import select 
import sys 
from os import path
from struct import *
from time import sleep

#definitions similar to server
def send_msg(sock, msg):
	toSend = bytes(msg, encoding="UTF-8")
	sock.sendall(toSend)

def recv_msg(sock):
	dataSize = sock.recv(4)
	dataSize = unpack("I", dataSize)

	#SOURCE: http://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
	return recvall(sock, dataSize[0])

# Writing to a file - SOUCE: https://realpython.com/read-write-files-python/
def recv_file(connection, filename):
	buffer = recv_msg(connection)
	if path.isfile(filename):
		filename = filename.split(".")[0] + "copy"

	with open(filename, "w") as f:
		f.write(buffer)	

# Get connection - bind socket
def connection_setup(ip, port):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.connect((ip, port))
    return server

def recvall(sock, n):
    received = ""
    while len(received) < n:
        packet = str(sock.recv(n - len(received)), encoding="UTF-8")
        if not packet:
            return None
        received += packet
    return received

# Function to get directory contents
# SOURCE: https://realpython.com/working-with-files-in-python/
def getDir(sock):
    data_size = sock.recv(4)
    data_size = unpack("I", data_size)
    received = str(sock.recv(data_size[0]), encoding="UTF-8").split("\x00")

    for val in received:
        print(val)

def sendNum(sock, message):
    to_send = pack('i', message)
    sock.send(to_send)

def req(conn, cmd, data):
    send_msg(conn, cmd + "\0")
    sendNum(conn, data)


#To run the main program
if __name__ == '__main__':
	# using same-as Project 1 
	# ensure number of arguments given is greater than 5 
	if len(sys.argv) < 5:
	    print("Correct usage: [file] [IP_address of host] [server-port] [command -l or -g] [filename or directory] [data-port] ")
	    exit(1) 

	#assign arguments to variables passed int
	IP_address = str(sys.argv[1]) 
	Port = int(sys.argv[2]) 
	Cmd = sys.argv[3]
	DataPort = 0
	file = ""


	# Check if cmd is -l or -g and rename arguments
	if len(sys.argv) is 5:
		DataPort = int(sys.argv[4])

	elif len(sys.argv) is 6:
		file = sys.argv[4]
		DataPort = int(sys.argv[5])

	# start server
	server = connection_setup(IP_address, Port)
	req(server, Cmd, DataPort)

	if Cmd == "-l":
		sleep(1)
		data = connection_setup(IP_address, DataPort)
		print("Retrieving directory from {}: {}".format(IP_address, DataPort))
		getDir(data)
		data.close()

	if Cmd == "-g":
		#SOURCE: to unpack - https://docs.python.org/3/library/struct.html
		sendNum(server, len(file))
		send_msg(server, file + "\0")

		result = recv_msg(server)

		if result == "File not found":
			print("{}: {} says {}".format(IP_address, Port, result))

		elif result == "File found!":
			print("Recieving \"{}\" from {}: {}".format(file, IP_address, DataPort))
			sleep(1)

			data = connection_setup(IP_address, DataPort)
			recv_file(data, file)
			print("File copy in directory - transfer complete.")
			data.close()

	server.close()

