/* $Id$ 
	pollcat.c
	handles all the polling shit

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


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/poll.h>
#include <kenmacros.h>
#include "display.h"
#include "common.h"

/* DEFS */
#define POLLTIMEOUT 5
#define READBUF 1024

/* hack around weirdness in ugh */
#ifndef INFTIM
#define INFTIM -1
#endif /* INFTIM */


/* GLOBS */
extern int shutdownFlag;

/***************
    NEWPFD
    Add a new socket to the list of polled fds 
****************/
static void
newPfd(struct pollfd *pfds, int * pfdCount, int fd, int events)
{
	DPRINTF(1, "adding fd %d to list... ", fd);
	pfds[*pfdCount].fd = fd;
	pfds[*pfdCount].events = events;
	(*pfdCount)++;
	DPRINTF(1, "now %d fd's in list\n", *pfdCount);

} /* END NEWPFD */


/******************
    REMOVEPFD
    Delete an fd from the list of polled fds.
  Decrease the count and close the gap in the array
*****************/
static void removePfd(struct pollfd *pfds, int * pfdCount, int fd)
{
  int i,j;

	DPRINTF(1, "removing fd %d from list...", fd);
    i=0;
    while (i<*pfdCount) {
        if (pfds[i].fd != fd) {
            i++;
            continue;
        }
        (*pfdCount)--;
        for (j=i;j<*pfdCount;j++){
            pfds[j] = pfds[j+1];
        }
    }
	DPRINTF(1, "now %d fd's left\n", *pfdCount);
} /* END REMOVEPFD */

/******************
    WRAPPEDWRITE
    a very ugly function to wrap the error conditions around write
    ok, it's overkill, since most of these packets are tiny, but whatever.
******************/
static int
wrappedWrite(int fd, void * buf, size_t count)
{
    size_t off = 0;
    size_t w = 0;

    while(off < count){
        w = write(fd, buf, count - off);
        if(w > 0){
            off += w;
        } else {
            switch(w){
                case EINTR:
                case EAGAIN: /* we're blocking, but you never know */
                    DPRINTF(2, "write got interrupted at %d...\n", off);
                    break;
                case 0:
                    DPRINTF(1, "broken pipe at %d...\n", off);
                    return EPIPE;
                    break;
                default:
                    return w; /* caller must error out */
                    break;
            } /* end swich */
        }
    }
    return w; /* shouldn't happen */
}/* END WRAPPEDWRITE */


/******************
	BCAST
	broadcast the whatever, out to all the other pfds
******************/
static int
bcast(char * buf, int len, int fromfd, struct pollfd * pfds, int numfds)
{
	int i  = 0;

	while(i  < numfds){
		if(pfds[i].fd != fromfd){
			DPRINTF(2, "bcast(): shouting out to my homey %s on fd %d\n",
				lookupName(pfds[i].fd), pfds[i].fd);
				/* XXX  i can't check for POLLOUT here, i might lose data!
					i may block here, waiting for write. that'd suck.  
					oh well. */
			RETCALL(wrappedWrite(pfds[i].fd, buf, len));	
		}

		i++;
	}
		
}/* END BCAST */



/************************
	PROCESSINPUT
	cycles through the fd's with stuff on 'em, and prints it
	RETURNS:  0 if ok, the fd number if it has closed, 
			and negative number if error
*************************/
static int 
processInput(struct pollfd * pfds, int numFds)
{
	char buf[READBUF];
	int rcount = 0;
	int i = 0;
	int whohad = 0; /* for debugging purposes */

	for (i = 0; i < numFds; i++){
		if(pfds[i].fd <0 ){
			continue;
		}
		if(pfds[i].revents & POLLIN ){
			/* ok. i don't wrap reads here, because i don't care 
				if i miss it, fuck it, i'll get it next time around */
			rcount = read(pfds[i].fd, buf, sizeof(buf)) ;
			if ( rcount > 0 ){
				/* shout it to all the others */
				bcast(buf, rcount, pfds[i].fd, pfds, numFds);

				RETCALL(display( pfds[i].fd, buf, rcount) );
				whohad++;
				continue;

			} else if (rcount < 0 ) {
				perror("read error in poll:processInput\n");
				return -1;
			} else { /* zero, read over, connection closed  */
				DPRINTF(1, "processInput(): connection %s closed on %d\n", 
						lookupName(pfds[i].fd), pfds[i].fd);
				/* XXX we blast outta here, whatever may have been 
				 * 		waiting on other fd's is lost. */
				return pfds[i].fd;
			}
		} /* end if */
	} /* end for */

	/* some debug  */
	if(whohad > 1){
		DPRINTF(2, "processInput(): we handled %d fd's\n", whohad);
	} else if (whohad < 1) {
		DPRINTF(2, "processInput(): nobody had data??\n");
	}
	return(0);

} /* END PROCESSINPUT */



/******************
	POLLLOOP
	takes an aarray of ints
******************/
static int
pollLoop(struct pollfd * pfds, int pfdCount)
{
	nfds_t foundCount; /* don't be scared, it's just a uint */
	int res = 0;

	DPRINTF(1, "pollLoop(): starting poll loop with total %d fd's\n", 
			pfdCount);

	while(pfdCount > 0){
		if(shutdownFlag != 0){
			return(0);
		}
		SYSCALL(foundCount = poll(pfds, pfdCount, INFTIM));
		assert(foundCount > 0); /* with INFTIM, should neer have 0 */

		RETCALL(res = processInput(pfds, pfdCount));
		if(res != 0){
			DPRINTF(1, "pollLoop(): got exit code of %d from processInput()\n",
				res);
			removePfd(pfds, &pfdCount, res); 
			/* TODO: if it's a network socket, close it? how? */
			/* TODO: re-open it? */
		}

	} /* end while */

	return (0); /* all done! */
	
}/* END POLLOOP  */



/******************
	TWOWAYPOLL
	point to point data
******************/
int
twoWayPoll(struct fdstruct ** lfds)
{
	/* TODO: this is duplication. 
	 * 		create a (void *) in fdstruct, and use it here */
	static struct pollfd pfds[3];
	static int pfdCount = 0; /* used by the newpfd utility function */
	struct fdstruct ** p;

   /* i have to initialize this first, since i don't do it in global decl */
    memset(pfds, -1, sizeof(pfds)); /* -1 is the skip flag for poll */

	for(p = lfds; *p != NULL; p++){
		SYSCALL(!isatty((*p)->fd));
		newPfd(pfds, &pfdCount, (*p)->fd, POLLIN);
	}

	pollLoop(pfds, pfdCount);

	return 0;
}/* END TWOWAYPOLL */



/* EOF */

