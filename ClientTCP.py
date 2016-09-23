# Creating September 2016 by Ian Thomas



import sys
import socket
import struct
import binascii

requestId = 0


def createPacket(Opcode, Oper1, Oper2):
    global requestId
    s = struct.Struct('B B B B h h')
    if Oper2 == '***':
        packet = (8, requestId, Opcode, 1, int(Oper1), Oper2)
        requestId += 1
        return packet

    else:
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
    operation = input()
    if operation == 'Q':
        exit()
    operation = getOptCode(operation)

    print('\nEnter your operand: ')
    operand1 = input()

    if operation != '<<' and operation != '>>':
        print('Enter your second operand: ')
        operand2 = input()

    return createPacket(operation, operand1, operand2)



def main(argv):
    if(len(sys.argv) != 3):
        print('Failed to provide Host name followed by Port number.\n')
        exit();
    request = True
    HOST = sys.argv[1]
    PORT = sys.argv[2]
    print(HOST + '\n' + PORT + '\n')

    #s = socket(socket.AF_INET, socket.SOCK_STREAM)
    #s.connect((HOST, PORT))
    print('Welcome to the calculating client\n')

    while(request):
        packet = interface()
        structure = struct.Struct('B B B B h h')
        print(packet)
        packed_data = structure.pack(*packet)
        print('Packed Value   :', binascii.hexlify(packed_data))
        s.sendall(packed_data)



if  __name__ == "__main__":
    main(sys.argv[1:])

