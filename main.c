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
#include "sock.h"

/* GLOBS */
int debug = 1;
extern char *optarg;
extern int optind, opterr, optopt;
int ptmx ; /* annoying, i hate having getopts options as globs. oh well */
     

/******************
	PARSEDEVICE
	calls the relevant open functions
	the open functions must parse out the : if need be
******************/
int
parseDevice(char * path)
{
	if(*path == '/'){
		return opentty(path);
	} else if ( strncmp(path, "pty", 3) == 0 ){
		return getPty(ptmx);
	} else { /* assume network socket */
		return opensock(path);
	}

}/* END PARSEDEVICE */

 

/***********************
	USAGE
	prints usage and dies
************************/
void
usage(){
	fprintf(stderr, "usage: sersnoop \n"
	"\t-a first device (default /dev/ttyS1:38400)\n"
	"\t-b second device (default pty)\n"
	"\t\t devices can be:\n"
	"\t\t\t'host:port', ipaddr:port' \n"
	"\t\t\t'/abs/path/to/ttyname:baud, or\n"
	"\t\t\t'pty' (it'll pick a pty for you)\n"
	"\t-d debug level\n"
	"\t-p use ptmx\n"
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
	int devAfd = -1;	
	int devBfd = -1;	
	int rv = 0;
	int sel = 0;
	int c;


	/* opts and such */
	while( (c= getopt(argc, argv, "pd:a:b:s")) != -1) {
		switch(c){
			case 'p':
				ptmx = 1;
				break;
			case 'a':
				RETCALL(devAfd = parseDevice(optarg));
				break;
			case 'b':
				RETCALL(devBfd = parseDevice(optarg));
				break;
			case 'd':
				debug = atoi(optarg);
				break;
			case 's':
				sel= 1;
				break;
			default:
				usage();
				break;
		} /* end switch */
	} /* end while */
	
	/* make sure the user specified all the required fields */
	if(devBfd < 0 || devAfd < 0){
		usage();
	}

	/* oh, why not */
	signalSetup();

	/* and now the loop de loop */
	rv = sel ? twoWaySelect(devAfd, devBfd) : twoWayPoll(devAfd, devBfd); 
	
	SYSCALL(close(devAfd));
	SYSCALL(close(devBfd));
	return rv;
}/* END MAIN */



/* EOF */

