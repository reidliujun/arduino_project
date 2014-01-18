'''
Created on Oct 19, 2013

@author: marius
'''
from socket import socket
import struct
import sys
import datetime
from threading import Thread
from pymongo import MongoClient

class Server(object):
    
    def __init__(self, s, db):
        self.socket = s
        self.db = db
        self.lightThreshold = 600;
        self.ledState = "unknown"
        self.pendingState = "unknown"
        self.lastSent = ""
        
    def storeValue(self, nodeId, value):
        entry = {"sensor_id": nodeId,
                 "value": value,
                 "created": datetime.datetime.utcnow()}
        self.db.statistics.insert(entry)
        
    def processValue(self, nodeId, value):
        print("led state before "  + self.ledState)
        if (value < self.lightThreshold):
            if (not self.ledState == "on"):
                self.pendingState = "on"
                self.ledState = "unknown"
                self.sendCommand("C 2 " + str(nodeId) + " 1023\r\n")
        else:
            if not self.ledState == "off":
                self.pendingState = "off"
                self.ledState = "unknown"
                self.sendCommand("C 2 " + str(nodeId) + " 0\r\n")
        print("led state now " + self.ledState)
   
    def sendCommand(self, command):
        print("Sending command " + command)
        self.socket.sendall(command)
        self.lastSent = command;
   
    def acknowledgement(self, nodeId, value):
        print("Got ack " + str(nodeId) + " " + str(value) + "  : " + self.lastSent)
        if not self.lastSent == "":
            params = self.lastSent.split()
            if (params[2] == nodeId and params[3] == value):
                self.lastSent = ""
                return True
            return False
                
    def serveOneClient(self):
        finish = False
        f = self.socket.makefile();
        while (not finish):
            try:
                command = f.readline().rstrip()
                print("Got command " + command)
            except Exception as e:
                print(e.message)
                finish = True
                continue
        
            if (command.startswith("Q") or command == ""):
                finish = True
                continue
            params = command.split()
            if (len(params) < 4): continue
            if (params[1] == "1"):
                nodeId = int(params[2])
                value = int(params[3])
                self.storeValue(nodeId, value)
                self.processValue(nodeId, value)
            elif (params[1] == "3"):
                ack = self.acknowledgement(params[2], params[3])
                if ack:
                    self.ledState = self.pendingState
                    print("Led state changed to " + self.ledState)
        self.socket.close();
    
#     def serveOneClient(self):
#         self.socket.sendall("Hello\n")
#         command = self.socket.recv(4096)
#         while (not command.startswith("quit")):
#             print (command)
#             self.socket.sendall(command.upper());
#             command = self.socket.recv(4096)
#             
#         self.socket.sendall("Bye\n")
#         self.socket.close()

def main():
    
    mongoClient = MongoClient('localhost', 27017)
    db = mongoClient.arduino
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_address = ('0.0.0.0', 8080)
    print >>sys.stderr, 'starting up on %s port %s' % server_address
    sock.bind(server_address)
    
    # Listen for incoming connections
    sock.listen(1)
    finish = False;
    while not finish:
    # Wait for a connection
        print >>sys.stderr, 'waiting for a connection'
        s, client_address = sock.accept()
        print >>sys.stderr, 'accepted connection from ', str(client_address)
        
        timeval = struct.pack("4I", 60*5, 0, 0, 0)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, timeval)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, timeval)
        try: 
            server = Server(s, db)
            t = Thread(target = server.serveOneClient)
            t.setDaemon(True)
            t.start();
        except Exception as e:
            print(e.__doc__)
            print(e.message)


if __name__ == '__main__':
    main()
    pass
