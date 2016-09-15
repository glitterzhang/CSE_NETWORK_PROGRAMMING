#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int SEND(int sockfd, const void *msg, int len, int flags,
         const struct sockaddr *to, int tolen)
{
    //sleep 10 milliseconds between sends
    usleep(10000);
    return sendto(sockfd, msg, len, flags, to, tolen);
}

int RECV(int socket, void *buf, int length,
         int flags, struct sockaddr *address,
         socklen_t *address_len)
{
    usleep(10000);
    return  recvfrom(socket, buf, length, flags,address, address_len);
}


