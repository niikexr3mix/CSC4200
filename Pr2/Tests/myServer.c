//server UDP

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
#include<arpa/inet.h>

#define BUFLEN 512

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
	struct sockaddr_in si_me, si_other;
	char buf[BUFLEN];
	char *filedump;
	int s, i, slen = sizeof(si_other) , recv_len;
	int portno;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		error("socket");
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	portno = atoi(argv[1]);
	filedump = (argv[2]);

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(portno);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		error("bind");
	}
	
	//keep listening for data
	while(1)
	{
		printf("Waiting for data...");
		fflush(stdout);
		
		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			error("recvfrom()");
		}
		
		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n" , buf);
		
		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
		{
			error("sendto()");
		}
	}

	close(s);
	return 0;
}