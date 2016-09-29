/*
** Modification of TCPServerDisplay to calculate the value
** received and send back the results rather than display
** the value received.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAXDATASIZE 100 // max number of bytes we can get at once

struct response {
  unsigned char tml;
  unsigned char request_id;
  unsigned char error_code;
  unsigned long result;
}__attribute__((__packed__));

typedef struct response response_t;

/*struct request {
  unsigned char tml;
  unsigned char request_id;
  unsigned char op_code;
  unsigned char num_operands;
  short op1;
  short op2;
}__attribute__((__packed__));

typedef struct request request_t;*/

void displayBuffer(char *Buffer, int length);

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// _M1 , now we need arguments int main(void)
int main(int argc, char *argv[]) // _M1 
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;

	char s[INET6_ADDRSTRLEN];
	int rv;

	int numbytes; // _M2
	char buf[MAXDATASIZE]; // _M2


	/* _M1 Begin */
	if (argc != 2) {
		fprintf(stderr,"usage: TCPServerDisplay Port# \n");
		exit(1);
	}
	/* _M1 End*/

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, argv[1] /* _M1 PORT*/, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
		  close(sockfd); // child doesn't need the listener
		  
      while (1) //allow multiple requests using one connection, don't force reconnect
      {
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
		      perror("recv");
		      exit(1);
		    }
        if (numbytes == 0)
        {
          break; //break if connection closes
        }
     
        uint32_t calculation;
        uint16_t op1, op2;
        unsigned char err = 0;
        switch (buf[2]) { //bitshift, buffer will be NBO (big endian)
          case 0 : // +
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 + (unsigned long)op2;
            break;
          case 1 : // -
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 - (unsigned long)op2;
            break;
          case 2 : // |
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 | (unsigned long)op2;
            break;
          case 3 : // &
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 & (unsigned long)op2;
            break;
          case 4 : // >>
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 >> (unsigned long)op2;
            break;
          case 5 : // <<       
            op1 = (buf[4] << 8) | buf[5];
            op2 = (buf[6] << 8) | buf[7];
            calculation = (unsigned long)op1 << (unsigned long)op2;
            break;
          default:
            calculation = 0;
            err = 127;      
        }

        response_t server_response;
        server_response.tml = 7;
        server_response.request_id = buf[1];
        server_response.error_code = err;
        server_response.result = htonl(calculation);

        if (send(new_fd, &server_response, 7, 0) == -1)
		      perror("send");
      }    
      close(new_fd);
		  exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

