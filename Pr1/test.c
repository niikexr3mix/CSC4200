//testClient.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void logg(const char *msg, char *filedump)
{
	FILE *fp;
	fp = fopen(filedump, "a");
	fprintf(fp, "%s", msg);
	fclose(fp);
}

int main(int argc, char *argv[])
{

	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *filedump;
    char str[256];
    //char hello[256] = "HELLO SERVER";

	//define the header for the packet to send
	struct headerPacket{
		int version;
		int messageType;
		int messageLength;
	};

	//define the packet to send
	struct commandPacket{
		struct headerPacket myHeader;
		char message[256];
	};

	// make the packet to send
	struct commandPacket tempPacket;
	tempPacket.myHeader.version = 17;

	//parse two command line args, port and log location
	if(argc < 3){
		fprintf(stderr, "ERROR, insufficient entries\n");
	}
	

	//turn port str into int
	//turn log location into str variable
	portno = atoi(argv[2]);
    filedump = (argv[3]);

    //connect to socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR connecting");
    }

    strcpy(tempPacket.message, "HELLO SERVER");
    n = send(sockfd, &tempPacket, sizeof(struct commandPacket), 0);

    n = recv(sockfd, &tempPacket, sizeof(struct commandPacket), 0);
    printf("MESSAGE FROM SERVER: ");
    printf(tempPacket.message, "\n");
    if(tempPacket.myHeader.version == 17){
    	logg("VERSION ACCEPTED...\n", filedump);

    	tempPacket.myHeader.messageType = 1;
    	n = send(sockfd, &tempPacket, sizeof(struct commandPacket), 0);
    	n = recv(sockfd, &tempPacket, sizeof(struct commandPacket), 0);
    	logg(tempPacket.message, filedump);

    	printf("\nSHUTTING DOWN...\n");
    }
    else{
    	logg("VERSION MISMATCH", filedump);
    }

    return 0;

    //n = recv(sockfd, tempPacket, sizeof(struct commandPacket), 0);
    //logg(tempPacket.message, filedump);

    
    //n = send(sockfd, tempPacket, sizeof(struct commandPacket), 0);
}