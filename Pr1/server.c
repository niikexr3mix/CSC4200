//server.c

/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
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
  fprintf(fp, "%s\n", msg);
  fclose(fp);
}

void dostuff (int sock, char *filedump)
{
  int n;
  char buffer[256];
    
  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n",buffer);
  n = write(sock,"I got your message",18);
  if (n < 0) error("ERROR writing to socket");
  //logg(buffer, filedump);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char *filedump;
    char *ipaddy;
    FILE *fp;

    struct headerS {
    int version = 17;
    int mesType;
    int mesLength;
    };

    struct packet {
      struct headerS h;
      char *mes[];
    };


    packet serverPacket;

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

     //ipaddy = ;
     fp = fopen(filedump, "a");
     fprintf(fp, "%s", "Received connection from IP: "); //find out how to get ip adress here
     //fprintf(fp, "%s" , ipaddy);
     fprintf(fp, "%s", " on Port: ");
     fprintf(fp, "%d\n", portno);
     fclose(fp);

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
             dostuff(newsockfd, filedump);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/