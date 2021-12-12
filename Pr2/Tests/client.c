//client

#include <errno.h>
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
#include <err.h>
#include <netdb.h>
#include <stdarg.h>

#define BUFLEN 2048
#define MSGS 5

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

void get_page(int s, char *host, char *page)
{
	char *msg;
	char *format = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: fetch.c\r\n\r\n";
	int status;

	status = asprintf (& msg, format, page, host);
	fail (status == -1 || ! msg, "asprintf failed.\n");
	status = send(s, msg, strlen (msg), 0);
	fail (status == -1, "send failed: %s\n", strerror (errno));
}

int main(int argc, char *argv[])
{
	struct sockaddr_in myaddr, remaddr;
	int fd, i, slen=sizeof(remaddr);
	char buf[BUFLEN];	/* message buffer */
	int recvlen, portNo;
	char *server, *filedump;
	char *host = "www.cprogramming.net";

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	server = (argv[1]);
	portNo = atoi(argv[2]);
    filedump = (argv[3]);

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
	{
		error("bind failed");
	}

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(portNo);
	if (inet_aton(server, &remaddr.sin_addr)==0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	get_page(s, host, "c/");

	/*for (i=0; i < MSGS; i++) 
	{
		printf("Sending packet %d to %s port %d\n", i, server, portNo);
		sprintf(buf, "This is packet %d", i);
		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		// now receive an acknowledgement from the server
		recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) 
        {
            buf[recvlen] = 0;	// expect a printable string - terminate it
            printf("received message: \"%s\"\n", buf);
        }
	}*/
	close(fd);
	return 0;
}