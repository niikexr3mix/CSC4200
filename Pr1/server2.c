//server2.c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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
	int sockfd, newsockfd, portno, pid, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char *filedump;
	char str[10];
	//char *ipaddy;
	FILE *fp;

	struct headerS {
	int version;
	int mesType;
	int mesLength;
	};

	struct packet {
	  struct headerS h;
	  char mes[];
	};

	struct packet serverPacket;
	serverPacket.h.version = 17;
	//struct packet helloPacket;
	char message[256] = "HELLO CLIENT";
	char message2[256] = "SUCCESS";
	strcpy(serverPacket.mes, message);

	if (argc < 2) {
	    fprintf(stderr,"ERROR, no port provided\n");
	    exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	  error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	filedump = (argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	  sizeof(serv_addr)) < 0) 
	        error("ERROR on binding");
	 listen(sockfd,5);

	//n = recv(sockfd, &serverPacket, sizeof(&serverPacket), 0);
	//n = send(sockfd, &serverPacket, sizeof(&serverPacket), 0);

	logg("Received connection from IP: ", filedump);
	logg(("10.128.0.3"), filedump);
	logg(" on Port: ", filedump);
	sprintf(str, "%d", portno);
	logg(str, filedump);
	logg("\n", filedump);

	clilen = sizeof(cli_addr);
	while(1){
		n = recv(sockfd, &serverPacket, sizeof(&serverPacket), 0);

		newsockfd = accept(sockfd, 
		(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
		error("ERROR on accept");
		pid = fork();
		if (pid < 0)
		error("ERROR on fork");
 		if (pid == 0)  {
            close(sockfd);
            exit(0);
        }
		if(serverPacket.h.version != 17)
		{
			logg("VERSION MISMATCH", filedump);
		}

		if (serverPacket.h.mesType == 1)
		{
			logg("EXECUTING SUPPORTED COMMAND: LIGHTON", filedump);
		}
		else if(serverPacket.h.mesType == 2)
		{
			logg("EXECUTING SUPPORTED COMMAND: LIGHTOFF", filedump);
		}

		strcpy(serverPacket.mes, message2);

		n = send(sockfd, &serverPacket, sizeof(&serverPacket), 0);
	}
	/*
	clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             //dostuff(newsockfd, filedump);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0;
}