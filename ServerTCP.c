/*






*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 10008


void get_server_info(struct addrinfo local_addr, struct addrinfo *serinfo);

int main(int argc, char *argv[]){

    int sockfd, new_fd, port_num;
    struct addrinfo local_addr, *servinfo, *i;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];

    if(strcmp(argv[1]), "10008" != 0){
        printf("That port number is not avaliable.")
        return 1;
    }

    memset(&local_addr, 0, sizeof local_addr);
    local_addr.ai_family = AF_UNSPEC;
    local_addr.ai_socktype = SOCK_STREAM;
    local_addr.ai_flags = AI_PASSIVE;
    get_server_info(&local_addr, &serv_info);
}


void get_server_info(struct addrinfo local_addr, struct addrinfo *serinfo){
    int rv;
    if(rv = getaddrinfo(NULL, PORT, &local_addr, &serinfo) != 0){
        fprintf(stdeer, "Failed to get server info : %s\n", gai_strerror(rv));
        exit(1);
    }

}
