/* $Id$ 
	serialmon.c
	does raw tty comms, forwards to/from pty, and prints out waz happenin.
*/

#include <fcntl.h>
#include <strings.h>
#include <termios.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <kenmacros.h>

#include "pollcat.h"
#include "serttys.h"
#include "serptys.h"
#include "sig.h"

/* GLOBS */
int debug = 1;
extern char *optarg;
extern int optind, opterr, optopt;

/**********************8
	USAGE
	prints usage and dies
************************/
void
usage(){
	fprintf(stderr, "usage: sersnoop \n"
	"\t-D terminal device (default /dev/ttyS1)\n"
	"\t-b terminal baudrate (default 38400)\n"
	"\t-d debug level\n"
	);
	exit(1);

} /* END USAGE */


/******************
	DECODEBAUD
******************/
int
decodeBaud()
{
	return(-1);
}/* END DECODEBAUD */



/******************
	MAIN
******************/
int
main(int argc, char ** argv)
{
	char * ttyPath = NULL ;
	char * slaveName = NULL ;
	int ptfd = -1;	
	int ttyfd = -1;	
	int baud = 38400;
	int rv = 0;
	int c;


	/* opts and such */
	while( (c= getopt(argc, argv, "d:D:b:")) != -1) {
		switch(c){
			case 'D':
				ttyPath = optarg;
				break;
			case 'd':
				debug = atoi(optarg);
				break;
			case 'b':
				if((baud = decodeBaud(atoi(optarg)) <1)){
					usage();
				}
				break;
			default:
				usage();
				break;
		} /* end switch */
	} /* end while */
	

	/* oh, why not */
	signalSetup();

	/* open the local pty */
	SYSCALL(ptfd = getPty(&slaveName));

	/* open the serial port */
	/* TODO: baud */
	ttyfd = opentty(ttyPath, B38400);

	/* and now the loop de loop */
	rv = twoWayPoll(ptfd, ttyfd); 
	
	/* eventually move the cleanups out, to be signal-clean */
	free(slaveName);
	SYSCALL(close(ptfd));
	SYSCALL(close(ttyfd));
	return rv;
}/* END MAIN */



/* EOF */

