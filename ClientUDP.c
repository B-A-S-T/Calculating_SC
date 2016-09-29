//	Created by Ian Thomas with the help of Riley Emnace and Andrew Turrentine September 2016
//	Client for Calculator, using UDP Socket Protocol
//	gcc -w ClientUDP.c -o client
//	./client HOSTNAME PORT

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

unsigned char request_id = 0;

// Struct for packet
struct packet{
    unsigned char tml;
    unsigned char request_id;
    unsigned char op_code;
    unsigned char num_operands;
    unsigned short op1;
    unsigned short op2;
}__attribute__((__packed__));

int getOp(char* operation);
struct packet createPacket(unsigned char Operand, unsigned short Oper1, unsigned short Oper2);

int main(int argc, char *argv[]){
    int i;
    if(argc != 3){
        printf("You must supply the IP address along with the port number\n");
        return 1;
    }

    int port;
    int sockfd;
    int rv;
    int numbytes;
    int requesting = 1;
    char buf[15];
    struct addrinfo hints, *servinfo, *p;
    struct packet packed_data;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    struct timeval start;
    struct timeval end;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0){
        fprintf(stderr, "Get address info: %s\n", gai_strerror(rv));
        return 1;
    }
    
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
        perror("Talker: socket");
        continue;
        }
        break;
    }

    if(p == NULL){
        fprintf(stderr, "talker:failed to create socket\n");
        return 2;
    }

    char operation[3];
    unsigned short op1;
    int length_rec;
    unsigned short op2; 

    //INTERFACE:
    printf("Welcome to the Calculating Client:\n");
    while(requesting){
    	printf("Choose your operation: (+), (-), (|), (&), (>>), (<<), Q to quit\n");
    	scanf("%s", operation);
    	if(strcmp(operation, "Q") == 0) return 1;
    	printf("What is your first operand: \n");
    	scanf("%hd", &op1);
    	printf("What is your second operand: \n");
    	scanf("%hu", &op2);
    	int opcode = getOp(operation);
    	packed_data = createPacket(opcode, op1, op2);
    
	clock_t t;
    	t = clock();

    	if((numbytes = sendto(sockfd, &packed_data, 8, 0, p->ai_addr, p->ai_addrlen)) == -1) {
       		perror("client: failed to send");
        	exit(1);
    	}
    
    	printf("Client sent: %d bytes to %s\n", numbytes, argv[1]);
    	addr_len = sizeof their_addr;
    	if ((length_rec = recvfrom(sockfd, buf, 15, 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
		}
   
    	t = clock() - t;
    	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    	unsigned long *value = buf+3;
    	*value = ntohl(*value);
  
    	printf("Answer: %d\n", *value);
    	printf("Request ID: %d\n", buf[1]);
    	printf("Round Trip: %f milliseconds\n", 
		time_taken);
    	}
    	freeaddrinfo(servinfo);
    	close(sockfd);
    	return 0;
}

int getOp(char* operation){
    if(strcmp(operation, "+") == 0) return 0;
    else if (strcmp(operation, "-") == 0) return 1;
    else if (strcmp(operation, "|") == 0) return 2;
    else if (strcmp(operation, "&") == 0) return 3;
    else if (strcmp(operation, ">>") == 0) return 4;
    else if (strcmp(operation, "<<") == 0) return 5;
}


struct packet createPacket(unsigned char Operation, unsigned short Oper1, unsigned short Oper2){
    struct packet packed_data;
    if(Operation == 4 || Operation == 5){
        packed_data.tml = 8;
        packed_data.request_id = request_id; 
        packed_data.op_code = Operation;
        packed_data.num_operands = 2;
        packed_data.op1 = Oper1;
        packed_data.op2 = Oper2;
        request_id += 1;
        return packed_data;
    }
        packed_data.tml = 8;
        packed_data.request_id = request_id; 
        packed_data.op_code = Operation;
        packed_data.num_operands = 2;
        packed_data.op1 = Oper1;
        packed_data.op2 = Oper2;
        request_id += 1;
        return packed_data;
}








