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
extern int shutdown;

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

   while(1){
		/* check for signal and shut down gracefully */
		if(shutdown){
			return(0);
		}
        SYSCALL(foundCount = poll(pfds, pfdCount, POLLTIMEOUT));
        if(foundCount == 0)
            continue;
        /* find and handle fd's with events pending */
        for(i = 0; i < pfdCount; i++){
			/* if(pfds[i].revents != POLLERR){
				DPRINTF(1, "unknown error on fd %d \n",
					pfds[i].fd);
			} */

			/* skip empties */
			if(pfds[i].revents != POLLIN)
				continue;


			if(pfds[i].fd == fd1){
				DPRINTF(1, "read %d chars from pfd %d\n", c, pfds[i].fd);
			} else if(pfds[i].fd == fd2){
				SYSCALL(c = read(pfds[i].fd, chunk, sizeof(chunk)));
			} /* end elses */
			DPRINTF(1, "read %d chars from pfd %d\n", c, pfds[i].fd);

		} /* end for */
	} /* end while */



	return 0;
}/* END TWOWAYPOLL */



/* EOF */

