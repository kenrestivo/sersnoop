/*	$Id$ 
	socket i/o functions

    Copyright (C) 2002  ken restivo <ken@restivo.org>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	OPENSERVER
	lifted from sockettest
******************/
int
openServer(char * path)
{
    struct sockaddr_in recvSa;   
    struct sockaddr_in peerSa;   
	struct in_addr recvIa;
	int port = 0;
	int sd = -1;
	int ad = -1;
	socklen_t saLen = sizeof(recvSa);
	socklen_t peerLen = sizeof(peerSa);

	/* get numeric port and error czech */
	port = strtoul(path, NULL, 0);
	if(port <10 || port > 65535){
		fprintf(stderr, "%d is a bad port number\n", port);
		return -1;
	}

	DPRINTF(1, "openServer(): opening socket listener on %d\n", port);

	/* sigh. */
    memset(&recvSa, 0, sizeof(recvSa)); 
    memset(&peerSa, 0, sizeof(peerSa)); 
    memset(&recvIa, 0, sizeof(recvIa)); 

	/* will i need to use the inet_addr macros? */
	recvIa.s_addr = INADDR_ANY;
    recvSa.sin_family = PF_INET;
    recvSa.sin_addr = recvIa;
	recvSa.sin_port = htons(port);
	
	/* bind the port and set up an accept loop */
	SYSCALL(sd = socket(PF_INET, SOCK_STREAM, 0)) ;
	SYSCALL(bind(sd, (struct sockaddr *)&recvSa, saLen) );
	SYSCALL(listen(sd, MAXTRIES));

	/* TODO: if i want the program to stay alive if connections die, 
			then this needs to go in the poll loop. non-trivial change!  */
	SYSCALL((ad = accept(sd, (struct sockaddr *)&recvSa, &saLen)));

	SYSCALL(getpeername(ad, &peerSa, &peerLen));
	DPRINTF(1, "got connection from %s on %d\n", 
		inet_ntoa(peerSa.sin_addr.s_addr), 
		ntohs(peerSa.sin_port));

	/* TODO: rip the gethostbyaddr() stuff from my old gethostfun program  */

	return ad;
}/* END OPENSERVER */



/******************
	OPENSOCK
	lifted from library program
******************/
int 
openSock(char * path)
{
    /* this is stupid. do with structs, as in the other one */
    struct sockaddr_in destSa;   
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
    memset(&destSa, 0, sizeof(destSa)); 
    memset(&destAddr, 0, sizeof(struct in_addr)); 

	/* resolve the address of the machine i'm trying to reach */
	assert(destHost != NULL && destPort >0 );
	if( (destHe = gethostbyname(destHost) ) == NULL){
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
				break;
		}
		return -1;
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
		fprintf(stderr, "can't connect to %s on %d: %s\n", 
			destHost, destPort, strerror(errno));
		if (i++ > MAXTRIES){
			fprintf(stderr, "after %d tries, gave up. argh.\n", i);
			return -1;
		}
		sleep(2);
	}
	DPRINTF(1, "now connected to %s on %d\n", 
		destHost, destPort);

#ifdef IPPROTO_TCP
	RETCALL(setsockopt(	sd,            /* socket affected */
					IPPROTO_TCP,     /* set option at TCP level */
					TCP_NODELAY,     /* name of option */
					(char *) &flag,  /* the cast is historical
											cruft */
					sizeof(int))    /* length of option value */
	);
#endif /* IPPROTO_TCP */


	return sd;
} /* END OPENSOCK */


/* EOF */

