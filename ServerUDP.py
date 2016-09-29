'''
    Simple udp socket server
'''
 
import socket
import sys
import struct

if len(sys.argv) != 2:
	print 'Usage: server portnumber'
	sys.exit()

HOST = socket.gethostname()
PORT = sys.argv[1] # Arbitrary non-privileged port
 
# Datagram (udp) socket
try :
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print 'Socket created'
except socket.error, msg :
    print 'Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
 
 
# Bind socket to local host and port
try:
    s.bind((HOST, int(PORT)))
except socket.error , msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
     
print 'Socket bind complete'
 
#now keep talking with the client
while 1:
	print '\n>>>> Waiting for request...'
	
    # receive data from client (data, addr)
	result = 0
	errCode = 0
	requestID = 0
	d = s.recvfrom(1024)
	data = d[0]
	addr = d[1]
    
	packet = data[:8]
	packet = struct.unpack("=BBBBhh", packet)
    
	#Perform operation based on op code.
	if int(packet[2]) == 0:
		result = int(packet[4]) + int(packet[5])
	elif int(packet[2]) == 1:
		result = int(packet[4]) - int(packet[5])
	elif int(packet[2]) == 2:
		result = int(packet[4]) | int(packet[5])
	elif int(packet[2]) == 3:
		result = int(packet[4]) & int(packet[5])
	elif int(packet[2]) == 4:
		result = int(packet[4]) >> int(packet[5])
	elif int(packet[2]) == 5:
		result = int(packet[4]) << int(packet[5])
	else:
		print 'Not a valid op code'
		errCode = 127
     
	if not data: 
		break
    
    #Package data for sending.
	responsePacket = (7, packet[1], errCode, result)
	structure = struct.Struct("! B B B L")
	packed_data = structure.pack(*responsePacket)
     
	s.sendto(packed_data , addr)
	print 'Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + data.strip()
     
s.close()
