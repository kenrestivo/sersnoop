/* $Id$ 
	selectloop.c
	deal with selecting

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
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <kenmacros.h>
#include "common.h"

extern int shutdownFlag;


/************************
	PROCESSINPUT
	cycles through the fd's with stuff on 'em, and prints it
	RETURNS:  negative number if error
*************************/
static int 
processInput(int * virtFd, int numFds, fd_set * foundFds)
{
	char buf[BUFSIZ];
	int rcount = 0;
	int i = 0;

	for (i = 0; i < numFds; i++){
		if(FD_ISSET(virtFd[i], foundFds)){
		/* 	TODO: copy/paste from pollcat.c!! */
			while ( (rcount = read(virtFd[i], buf, sizeof(buf)) ) >0 ){
				/* buncha UGLY debug stuff */
				#ifdef __SVR4
				DPRINTF(1, "processInput(): got %d bytes from fd %d: ",
					rcount, virtFd[i]  );
				#else
				DPRINTF(1, "processInput(): got %d bytes from fd %d, %s: ",
					rcount, virtFd[i], ttyname(virtFd[i]) );
				#endif /* __SVR4 */

				if(write(1, buf, rcount) < 0){
					fprintf(stderr, "processInput(): write problem\n");
					return -2;
				}
			} /* end while */
		} /* end if */
	} /* end for */
	return 0;
} /* END PROCESSINPUT */



/************************
	SELECTLOOPER
	sits in a select loop and waits for input
	virtFs is an array of file descriptors
	RETURNS: status
************************/
static int 
selectLooper(int * virtFd, int numFds)
{
	fd_set listenFds;
	fd_set foundFds;
	struct timeval timeout;
	int maxListenFds = 0;
	int numActive = 0;
	int i = 0;

	DPRINTF(1, "starting select loop with %d fd's\n", numFds);
	
	/* clean out listenfds and set up timer */
	FD_ZERO(&listenFds);
	timeout.tv_usec = 0;
	timeout.tv_sec = 5;
	
	/* fill up my master listenfd set  and generate maxfds*/
	for (i = 0; i < numFds; i++){
		FD_SET(virtFd[i], &listenFds);	
		maxListenFds = (virtFd[i] > maxListenFds) ? virtFd[i] : maxListenFds;
	}
	maxListenFds += 1;
	
	/* select loop */
	while(!shutdownFlag){
		/* reset my foundFds equal to the listenFds (i don't FD_ZERO them) */
		foundFds = listenFds;
		
		/* do the read select  and handle errors */
		numActive = select(maxListenFds, &foundFds, NULL, NULL, &timeout);
		if (numActive <0){ 
			fprintf(stderr, "selectLoop: select %d : %s\n", 
				numActive,  strerror(errno));
			return numActive;
		} else if (numActive > 0){
			/* i got something. switch on the return set */
			/* TODO, find a way to pass errors along cleaner */
			if (processInput(virtFd, numFds, &foundFds) <  0){
				fprintf(stderr, "selectLoop: getting input %d : %s\n", 
					numActive,  strerror(errno));
				return -3 ;
			}	
		}

	} /* end select loop */
	
	return (0);

} /* END SELECTLOOPER */



/******************
	TWOWAYSELECT
	starts the loop spinning, with ttwo fd's
******************/
int
twoWaySelect(struct fdstruct ** lfds)
{
	int virtFd[2];

	/* XXX cheap and dirty. 
		i don't bother cycling through them all, just assume there's two */
	virtFd[0] = lfds[0]->fd;
	virtFd[1] = lfds[1]->fd;

	return selectLooper(virtFd, 2);
	
}/* END TWOWAYSELECT  */





/* EOF */

