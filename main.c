/* $Id$ 
	main.c for serialsnoop
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
#include "selectloop.h"

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
	"\t-s try it with select not poll (for debugging)\n"
	);
	exit(1);

} /* END USAGE */



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
	int baudcode = 0xF; /* 38400 by default */
	int rv = 0;
	int sel = 0;
	int c;

	/* defaults */
	ttyPath = strdup("/dev/ttyS1");

	/* opts and such */
	while( (c= getopt(argc, argv, "d:D:b:s")) != -1) {
		switch(c){
			case 'D':
				free(ttyPath);
				ttyPath= NULL;
				ttyPath = optarg;
				break;
			case 'd':
				debug = atoi(optarg);
				break;
			case 's':
				sel= 1;
				break;
			case 'b':
				if((baudcode = decodeBaud(atoi(optarg)) <1)){
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
	ttyfd = opentty(ttyPath, baudcode);

	/* and now the loop de loop */
	rv = sel ? twoWaySelect(ptfd, ttyfd) : twoWayPoll(ptfd, ttyfd); 
	
	/* eventually move the cleanups out, to be signal-clean */
	free(slaveName);
	SYSCALL(close(ptfd));
	SYSCALL(close(ttyfd));
	return rv;
}/* END MAIN */



/* EOF */

