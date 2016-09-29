# Created by Ian Thomas with the help of Riley Emnace and Andrew Turrentine September 2016
# python ClientTCP.py HOSTNAME PORT

import time
import sys
import socket
import struct
import binascii
from timeit import Timer

requestId = 0


def createPacket(Opcode, Oper1, Oper2):
    global requestId
    packet = (8, requestId, Opcode, 2, int(Oper1), int(Oper2))
    requestId += 1
    return packet


def getOptCode(opt):
    if opt == '+':
        return 0
    elif opt == '-':
        return 1
    elif opt == '|':
        return 2
    elif opt == '&':
        return 3
    elif opt == '>>':
        return 4
    elif opt == '<<':
        return 5

def interface():
    opt2 = '***'
    print('Enter an operation: (+), (-), (|), (&), (>>), (<<), or Q to quit:')
    operation = raw_input()
    if operation == 'Q':
        exit()
    operation = getOptCode(operation)

    print('\nEnter your operand: ')
    operand1 = raw_input()

    print('Enter your second operand: ')
    operand2 = raw_input()

    return createPacket(operation, operand1, operand2)



def main(argv):
    if(len(sys.argv) != 3):
        print('Failed to provide Host name followed by Port number.\n')
        exit();

    request = True
    data = ''
    HOST = sys.argv[1]
    PORT = sys.argv[2]
    print(HOST + '\n' + PORT + '\n')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, int(PORT)))

    print('Welcome to the calculating client\n')
    while(request):
        packet = interface()
        structure = struct.Struct('! B B B B h h')
        packed_data = structure.pack(*packet)
        #print('Packed Value   :', binascii.hexlify(packed_data))
	start = int(round(time.time() * 1000))
        s.sendall(packed_data)

	data = s.recv(1024)
	calculation = data[3:]
	calculation = struct.unpack('>L', calculation)
	request = data[1]
	request = struct.unpack('>B', request)
	end = int(round(time.time() * 1000))
	
	print 'Value: ', calculation[0]
	print 'RequestId: ', request[0]
	print end - start, 'ms' 

if  __name__ == "__main__":
    main(sys.argv[1:])

