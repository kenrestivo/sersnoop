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
******************/
int
twoWayPoll(int fd1, int fd2)
{
	static struct pollfd pfds[3];
	static int pfdCount ; /* used by the newpfd utility function */
	nfds_t foundCount;
	char chunk[READBUF];
	int c;
	int i;

   /* i have to initialize this first, since i don't do it in global decl */
    pfdCount = 0;
    memset(pfds, -1, sizeof(pfds)); /* -1 is the skip flag for poll */

    newPfd(pfds, &pfdCount, fd1, POLLIN);
    newPfd(pfds, &pfdCount, fd2, POLLIN);

   while(1){
        SYSCALL(foundCount = poll(pfds, pfdCount, POLLTIMEOUT));
        if(foundCount == 0)
            continue;
        /* find and handle fd's with events pending */
        for(i = 0; i < pfdCount; i++){
			if(pfds[i].revents != POLLERR){
				DPRINTF(1, "whoops! error on fd %d ",
					pfds[i].fd);
				/* this should force it to fail and set errno */
				RETCALL(c = read(pfds[i].fd, chunk, sizeof(chunk)));
			}

			/* skip empties */
			if(pfds[i].revents != POLLIN)
				continue;

			SYSCALL(c = read(pfds[i].fd, chunk, sizeof(chunk)));
			DPRINTF(1, "read %d chars from pfd %d\n", c, pfds[i].fd);

			if(pfds[i].fd == fd1){
			} else if(pfds[i].fd == fd2){
			} /* end elses */

		} /* end for */
	} /* end while */



	return 0;
}/* END TWOWAYPOLL */



/* EOF */

