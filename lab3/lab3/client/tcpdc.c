//TCPS is accepting datagrams from ftpc and sending to Troll

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>


#define LOCAL_PORT 4001


//Structure used by troll
typedef struct MyMessage {
	struct sockaddr_in header;							//Header for final destination address
	struct tcp tcpheader;
	char body[1000]; 									//Actual data
} MyMessage;

main(int argc, char const *argv[])
{
	MyMessage message;									//Packet format accepted by troll
	int sock, troll_sock;                               //Initial socket descriptors
	struct sockaddr_in troll, my_addr;					//Structures for server and tcpd socket name setup

    //If there are more or less than 3 arguments show error
    //First argument: exec file         Second argument: local tcpd port number
    //Third argument: local troll port number 
    if (argc!=2){
        printf("Usage: %s <troll-port>\n",argv[0]);
        exit(1);
    }

    //Initialize socket for UDP in linux
    printf("Setting up socket...\n");
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    	perror("Error openting datagram socket");
    	exit(1);
    }
    printf("Socket initialized \n");

    //Copying socket to send to troll
    troll_sock = sock;

    //Constructing socket name for receiving
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;			//Listen to any IP address
	my_addr.sin_port = htons(LOCAL_PORT);

    //Constructing socket name of the troll to send to
  	troll.sin_family = AF_INET;
  	troll.sin_port = htons(atoi(argv[1]));
  	troll.sin_addr.s_addr = inet_addr("127.0.0.1");

  	//Binding socket name to socket
	printf("Binding socket to socket name...\n");
	if (bind(sock, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("Error binding stream socket");
		exit(1);
	}
	printf("Socket name binded, waiting...\n");

	//To hold the length of my_addr
	int len = sizeof(my_addr);

	//Counter to count number of datagrams forwarded
	int count = 0;

	//Always keep on listening and sending
	while(1) {

		//Receiving from ftpc
		int rec = recvfrom(sock,&message, sizeof(message), 0, (struct sockaddr *)&my_addr, &len);

		if(rec<0){
			perror("Error receiving datagram");
			exit(1);
		}

		//calculate checksum and save it in tcpheader

		int len=rec;
		uint16_t checksum=crc16(0,message,len);
        message.tcpheader.checksum=htons(checksum);


		printf("Received data, sending to troll --> %d\n",count);

		//Sending to troll
		int s = sendto(troll_sock,&message,sizeof(message),0, (struct sockaddr *)&troll, sizeof(troll));

        if (s < 0)
        {
            perror("Error sending datagram");
            exit(1);
        }

        //Incrementing counter
        count++;

	}

	//Close the sockets
	close(sock);
	close(troll_sock);
}