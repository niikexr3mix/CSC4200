//server

#include <stdio.h>	/* defines printf */
#include <stdlib.h>	/* defines exit and other sys calls */
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 2048

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
	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	int recvlen;
	int fd, portNo, msgcnt;
	unsigned char buf[BUFSIZE];
	char *filedump;

	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		error("Error! Cannot Create Socket!");
	}

	portNo = atoi(argv[1]);
	filedump = (argv[2]);

	//printf("created socket: descriptor=%d\n", fd);

	memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(portNo);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
    {
        error("bind failed");
	}

	while(1)
	{
		printf("waiting on port %d\n", portNo);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;
			printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
		}
		else
			printf("uh oh - something went wrong!\n");
		sprintf(buf, "ack %d", msgcnt++);
		printf("sending response \"%s\"\n", buf);
		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			perror("sendto");
	}

		/*
        printf("received %d bytes\n", recvlen);
        if (recvlen > 0) 
        {
            buf[recvlen] = 0;
            printf("received message: \"%s\"\n", buf);
            sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
        }
    }*/

}