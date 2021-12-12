

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


//dostuff header
void dostuff (int sock, char *filedump, int portno, char str1[]);

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
	char *ipaddress;

	struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&cli_addr;
	struct in_addr ipAddr = pV4Addr->sin_addr;

	// make the packet to send
	//struct commandPacket tempPacket;
	//tempPacket.myHeader.version = 17;

	//parse two command line args, port and log location
	if(argc < 2){
		fprintf(stderr, "ERROR, insufficient entries\n");
	}

	//create the socket connection
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		error("ERROR opening socket");
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));

	//turn port str into int
	portno = atoi(argv[1]);

	//turn ip address into string
	char str1[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &ipAddr, str1, INET_ADDRSTRLEN );
	//printf("%s", str1);

	//turn log location into str variable
	filedump = (argv[2]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	//begin listen on server
	listen(sockfd, 5);

	clilen = sizeof(cli_addr);

	//printf(cli_addr);

	//begin listening loop on connection
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if(newsockfd < 0){
			error("ERROR on accept");
		}
		pid = fork();
		if (pid < 0){
            error("ERROR on fork");
        }
        if (pid == 0)  {
            close(sockfd);
            dostuff(newsockfd, filedump, portno, str1);
            exit(0);
        }
         else close(newsockfd);
	}
	close(sockfd);
	return 0;
}



void dostuff (int sock, char *filedump, int portno, char str1[])
{
	int n;
	char buffer[10];
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
	struct commandPacket serverPacket;
	sprintf(buffer, "%d", portno);

	//log the connection ip and port from the client if it recieves
	logg("Received connection from IP: ", filedump);
	logg((str1), filedump);
	logg(" on Port: ", filedump);
	logg(buffer, filedump);
	logg("\n", filedump);

	n = recv(sock, &serverPacket, sizeof(struct commandPacket), 0);
	printf("MESSAGE FROM CLIENT: ");
	printf(serverPacket.message, "\n");

	strcpy(serverPacket.message, "HELLO CLIENT");
	n = send(sock, &serverPacket, sizeof(struct commandPacket), 0);
	n = recv(sock, &serverPacket, sizeof(struct commandPacket), 0);

	if(serverPacket.myHeader.version == 17){
		if(serverPacket.myHeader.messageType == 1){
			logg("EXECUTING SUPPORTED COMMAND: LIGHTON\n", filedump);
		}
		else if(serverPacket.myHeader.messageType == 2){
			logg("EXECUTING SUPPORTED COMMAND: LIGHTOFF\n", filedump);
		}
		else{
			logg("IGNORING UNKNOWN COMMAND", filedump);
		}

		strcpy(serverPacket.message, "SUCCESS\n");
		n = send(sock, &serverPacket, sizeof(struct commandPacket), 0);

		printf("\nNEXT CLIENT\n");

	}
	else{
		logg("ERROR version mismatch", filedump);
	}
  
/*



     //n = recv(sock, buffer, sizeof(buffer), 0);

  //logg(buffer, filedump);
  //tempPacket.message = "HELLO CLIENT!";
  //n = send(sock, tempPacket.message, sizeof(struct tempPacket), 0);
  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n",buffer);
  n = write(sock,"I got your message",18);
  if (n < 0) error("ERROR writing to socket");
  */
  }