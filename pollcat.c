/* $Id$ 
	pollcat.c
	handles all the polling shit
*/


#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/poll.h>
#include <kenmacros.h>
#include "display.h"

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

  pfds[*pfdCount].fd = fd;
  pfds[*pfdCount].events = events;
  (*pfdCount)++;

} /* END NEWPFD */


/******************
    REMOVEPFD
    Delete an fd from the list of polled fds.
  Decrease the count and close the gap in the array
*****************/
static void removePfd(struct pollfd *pfds, int * pfdCount, int fd)
{
  int i,j;

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
} /* END REMOVEPFD */


/************************
	PROCESSINPUT
	cycles through the fd's with stuff on 'em, and prints it
	RETURNS:  negative number if error
*************************/
static int 
processInput(struct pollfd * pfds, int numFds)
{
	char buf[READBUF];
	int rcount = 0;
	int i = 0;

	for (i = 0; i < numFds; i++){
		if(pfds[i].revents & POLLIN ){
			/* XXX  should i while() here, or just grab one chunk and leave? */
			while ( (rcount = read(pfds[i].fd, buf, sizeof(buf)) ) >0 ){
				/* buncha UGLY debug stuff */
				#ifdef __SVR4
				DPRINTF(1, "got %d bytes from fd %d: ",
					rcount, pfds[i].fd  );
				#else
				RETCALL(display( ttyname(pfds[i].fd), buf, rcount) );
				#endif /* __SVR4 */

			} /* end while */
#if 0
			if(rcount < 0){
				perror("read error in poll:processInput\n");
			}
#endif
		} /* end if */
	} /* end for */
	return 0;

} /* END PROCESSINPUT */



/******************
	POLLLOOP
	takes an aarray of ints
******************/
static int
pollLoop(struct pollfd * pfds, int pfdCount)
{
	nfds_t foundCount; /* don't be scared, it's just a uint */

	DPRINTF(1, "starting poll loop with total of %d fd's\n", pfdCount);

	while(shutdownFlag == 0){
		SYSCALL(foundCount = poll(pfds, pfdCount, INFTIM));
		assert(foundCount > 0); /* with INFTIM, should neer have 0 */

		processInput(pfds, pfdCount);

	} /* end while */

	return (0);
	
}/* END POLLOOP  */



/******************
	TWOWAYPOLL
	point to point data
******************/
int
twoWayPoll(int fd1, int fd2)
{
	static struct pollfd pfds[3];
	static int pfdCount = 0; /* used by the newpfd utility function */

	SYSCALL(!isatty(fd1));
	SYSCALL(!isatty(fd2));

   /* i have to initialize this first, since i don't do it in global decl */
    memset(pfds, -1, sizeof(pfds)); /* -1 is the skip flag for poll */

    newPfd(pfds, &pfdCount, fd1, POLLIN);
    newPfd(pfds, &pfdCount, fd2, POLLIN);

	pollLoop(pfds, pfdCount);

	return 0;
}/* END TWOWAYPOLL */



/* EOF */

