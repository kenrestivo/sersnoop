/*	$Id$ 
	socket i/o functions
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <kenmacros.h>
#include "util.h"
#include "sock.h"


#define MAXBUF  1024
#define MAXTRIES  5

extern int h_errno;



/******************
	OPENSOCK
	lifted from library program
******************/
int 
opensock(char * path)
{
    /* this is stupid. do with structs, as in the other one */
    struct sockaddr_in destSa;   
    char myname[MAXBUF];
	struct hostent * destHe = NULL;
	extern int h_errno;
	struct in_addr destAddr ;
	unsigned long int * tempintptr;
	char * destHost;
	int  destPort;
	int i = 0;
	int sd = -1;
	int flag = 1;

	RETCALL( destPort = splitColon(path, &destHost) );

	/* zero out bits */
    memset(&destSa, 0, sizeof(struct sockaddr_in)); 
    memset(&destAddr, 0, sizeof(struct in_addr)); 

	/* not really needed for a client */
	RETCALL(gethostname(myname, (size_t)sizeof(myname)));

	/* resolve the address of the machine i'm trying to reach */
	assert(destHost != NULL && destPort >0 );
	NULLCALL( (destHe = gethostbyname(destHost) ) );
	switch(h_errno) {
		case HOST_NOT_FOUND:
			fputs("Host not found.\n", stderr);
			break;
		case TRY_AGAIN:
			fputs("Transient failure.\n", stderr);
			break;
		case NO_RECOVERY:
			fputs("No recovery, whatever that means.\n", stderr);
			break;
		case NO_ADDRESS:
			fputs("No address found.\n", stderr);
			break;
		default: fputs("Unknown error.\n", stderr);
			return -1;
			break;
	}

	RETCALL (sd = socket(PF_INET, SOCK_STREAM, 0)) ;

	/* obnoxiousness */
	tempintptr =  (unsigned long int*) *destHe->h_addr_list;
	destAddr.s_addr = *tempintptr ;
	
	/* fill the sockaddr struct */
    destSa.sin_family = PF_INET;
	destSa.sin_addr = destAddr;
	destSa.sin_port = htons(destPort);


	/* try to connect, a couple times */
	i = 0;
	while (
		connect(sd, 
		(struct sockaddr *)&destSa, 
		sizeof(struct sockaddr_in)) < 0 )
	{
		fprintf(stderr, "can't connect to %s on %d from %s: %s\n", 
			destHost, destPort, myname, strerror(errno));
		if (i++ > MAXTRIES){
			fprintf(stderr, "after %d tries, gave up. argh.\n", i);
			return -1;
		}
		sleep(2);
	}
	DPRINTF(1, "now connected to %s on %d from %s\n", 
		destHost, destPort, myname);

	RETCALL(setsockopt(	sd,            /* socket affected */
					IPPROTO_TCP,     /* set option at TCP level */
					TCP_NODELAY,     /* name of option */
					(char *) &flag,  /* the cast is historical
											cruft */
					sizeof(int))    /* length of option value */
	);


	return sd;
} /* END OPENSOCK */


/* EOF */

