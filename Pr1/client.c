//client.c

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
    exit(0);
}

void logg(const char *msg, char *filedump)
{
  FILE *fp;
  fp = fopen(filedump, "a");
  fprintf(fp, "%s\n", msg);
  fclose(fp);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *filedump;
    char str[256];

    struct headerS {
    	int version;
    	int mesType;
    	int mesLength;
    };
	struct packet {
    	struct headerS h;
    	char mes[];
    };

    struct packet *tPacket;
    tPacket->h.version = 17;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    filedump = (argv[3]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    //n = send(sockfd, tPacket, sizeof(*tPacket)+1, 0);

    //sprintf(str, "%d" ,n);
    //logg(str, filedump);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    strcpy(tPacket->mes, buffer);
    (tPacket->h.mesLength) = strlen(tPacket->mes);

    n = send(sockfd, tPacket, sizeof(*tPacket)+1, 0);
    //n = write(sockfd,buffer,strlen(buffer));

    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);

    n = recv(sockfd, tPacket, sizeof(*tPacket)+1, 0);
    //n = read(sockfd,buffer,255);

    if(tPacket->h.version == 17) {
    	logg("VERSION ACCEPTED", filedump);
    }
    else {
    	logg("VERSION MISMATCH", filedump);
    	return 0;
    }

    printf("Please select your command (LIGHTON = 1 / LIGHTOFF = 2): ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    (tPacket->h.mesType) = atoi(buffer);

    n = send(sockfd, tPacket, sizeof(*tPacket)+1, 0);

    n = recv(sockfd, tPacket, sizeof(*tPacket)+1, 0);
    logg("SUCCESS", filedump);

    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    logg(buffer, filedump);
    close(sockfd);
    return 0;
}