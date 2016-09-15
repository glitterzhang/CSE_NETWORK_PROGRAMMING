
//TCPS is accepting datagrams from ftpc and sending to Troll

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <linux/tcp.h>

#include "Header.h"
#define LOCAL_PORT 5001

//Structure used by troll
typedef struct MyMessage {
	struct sockaddr_in header;							//Header for final destination address
	struct tcphdr tcpheader;
	char body[1000]; 									//Actual data
} MyMessage;

main(int argc)
{
	MyMessage message;									//Packet format sent by troll
	int sock, server_sock;								//Initial socket descriptors
	struct sockaddr_in my_addr, server_addr;			//Structures for server and tcpd socket name setup
	char buff[1000];									//Buffer for holding data

    //Initialize socket for UDP in linux
    printf("Setting up socket...\n");
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    	perror("Error openting datagram socket");
    	exit(1);
    }
    printf("Socket initialized \n");

    //Copying socket to send to ftps
    server_sock = sock;

    //Constructing socket name for receiving
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;			//Listen to any IP address
	my_addr.sin_port = htons(LOCAL_PORT);

	//Constructing socket name for sending to ftps
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  	//Binding socket name to socket
	printf("Binding socket to socket name.\n");
	if (bind(sock, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("Error binding stream socket");
		exit(1);
	}
	printf("Socket name binded, waiting now.\n");

	//To hold the length of my_addr
	int len = sizeof(my_addr);

	//To hold the port number sent by ftps
	char port[4];

	//Always keep on listening
	while(1) {

		//Receiving from ftps for the port number
		int rec = recvfrom(sock,&buff, 1000, 0, (struct sockaddr *)&my_addr, &len);

		if(rec<0){
			perror("Error receiving datagram");
			exit(1);
		}

		//Copying buffer to port
		memcpy(port,buff,1000);

		//Setting port number in struct
		server_addr.sin_port = htons(atoi(port));

		printf("New server connected at port: %s\n", port);
        printf("Please connect with port 5001.\n");

		printf("Sending all packets to the server...\n");

		//Counter to count number of datagrams forwarded
		int count = 0;

		//Always keep on listening and sending
		while(1) {

			//Receiving from troll
			int rec = recvfrom(sock,&message, sizeof(message), 0, (struct sockaddr *)&my_addr, &len);

			if(rec<0){
				perror("Error receiving datagram");
				exit(1);
			}
		
             //Calculate checksum
            //tcpheader =20byte socket adrress header 16 byte
            uint16_t sent_checksum = ntohs(message.tcpheader.check);// get checksum from message packet
            bzero((void*)&message.tcpheader.check,2);

            unsigned char * data = malloc(rec-sizeof(message.header));
            memcpy(data, &message.tcpheader,rec-sizeof(message.header));// can't use &message+16
	   	    uint16_t reciev_checksum=crc16(0,data,rec-sizeof(message.header));

             //unsigned char * r_data = malloc(rec-sizeof(message.header));
            //printf("sizeof redata%d\n", rec-sizeof(message.header));
             //memcpy(r_data, &message.tcpheader,rec-sizeof(message.header));
              //printf("sizeof data%s\n",rec-sizeof(message.header));

            // uint16_t reciev_checksum =crc16(0,r_data,rec-sizeof(message.header));

             //compare checksum
             if(reciev_checksum!=sent_checksum)
             {
             	printf("%s\n","This packet was garbled");
             }
               else
               { 
               	printf("%s\n","checksum is ok" );
             }
             printf("%s\n","send checksum is");
             printf("%u\n",sent_checksum);
             printf("%s\n","recieve checksum is");
             printf("%u\n",reciev_checksum);

           
			////Sending to ftps
			int s = sendto(server_sock,message.body,sizeof(message.body),0, (struct sockaddr *)&server_addr, sizeof(server_addr));

	        if (s < 0)
	        {
	            perror("Error sending datagram");
	            exit(1);
	        }

	        printf("Received and sent --> %d\n",count);

	        //Incrementing counter
	        count++;

		}
	}

	//Close the sockets
	close(sock);
	close(server_sock);
}
