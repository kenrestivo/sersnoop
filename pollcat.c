/* $Id$ 
	pollcat.c
	handles all the polling shit
*/

#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <kenmacros.h>

/* DEFS */
#define POLLTIMEOUT 5
#define READBUF 1024

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



/******************
	TWOWAYPOLL
	point to point data
	XXX: note this function is plain-ol' broken. DFW: don't fucking work.
******************/
int
twoWayPoll(int fd1, int fd2)
{
	static struct pollfd pfds[3];
	static int pfdCount = 0; /* used by the newpfd utility function */
	nfds_t foundCount;
	char chunk[READBUF];
	int c;
	int i;

	SYSCALL(!isatty(fd1));
	SYSCALL(!isatty(fd2));

   /* i have to initialize this first, since i don't do it in global decl */
    memset(pfds, -1, sizeof(pfds)); /* -1 is the skip flag for poll */

    newPfd(pfds, &pfdCount, fd1, POLLIN);
    newPfd(pfds, &pfdCount, fd2, POLLIN);

	/* XXX hello? it's all gone */

	return 0;
}/* END TWOWAYPOLL */



/* EOF */

