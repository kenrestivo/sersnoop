/* $Id$ 
	main.c for serialsnoop
	does raw tty comms, forwards to/from pty, and prints out waz happenin.

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
#ifdef USE_SSL
#include "sslsock.h"
#endif /* USE_SSL */
#include "display.h"
#include "common.h"

/* GLOBS */
int debug = 1;
extern char *optarg;
extern int optind, opterr, optopt;
int ptmx ; /* annoying, i hate having getopts options as globs. oh well */
struct fdstruct * gfds[3];  /* null-term array of structs. */
     

/******************
	PARSEDEVICE
	calls the relevant open functions
	the open functions must parse out the : if need be
******************/
struct fdstruct *
parseDevice(char * path)
{
	/* CASE: tty */
	if(*path == '/'){ 
		return opentty(path);
	} else if ( strncmp(path, "pty", 3) == 0 ){
		return getPty(ptmx);
	/* CASE: network server */
	} else if (strchr(path,':') == NULL){ 
		if(path[strlen(path)-1] == 'S'){
#ifdef USE_SSL
			return openSSLServer(path);
#else /* USE_SSL */
		fprintf(stderr, "%s: ssl support not compiled in\n", path);
		exit(1);
#endif /* USE_SSL */

		} else {
			return openServer(path);
		}
	/* CASE network client */
	} else { 
		if(path[strlen(path)-1] == 'S'){
#ifdef USE_SSL
			return openSSLSock(path);
#else /* USE_SSL */
		fprintf(stderr, "%s: ssl support not compiled in\n", path);
		exit(1);
#endif /* USE_SSL */
		} else {
			return openSock(path);
		}
	}

}/* END PARSEDEVICE */

 

/***********************
	USAGE
	prints usage and dies
************************/
void
usage(){
	fprintf(stderr, "sersnoop version " REL 
#ifdef USE_SSL
	" with SSL\n"
#endif /* USE_SSL */
	"\n"
	"\t-a first device (default /dev/ttyS1:38400)\n"
	"\t-b second device (default pty)\n"
	"\t either or both devices can be:\n"
	"\t\t<host:port> or <ipaddr:port> for client (or <host:port>S for SSL-based client)\n"
	"\t\t<port> for server (or <port>S for SSL-based server\n"
	"\t\t</abs/path/to/ttyname:baud>, for tty or\n"
	"\t\t<pty> (it'll pick a pty for you)\n"
	"\t-d debug level\n"
	"\t-p use ptmx\n"
	"\t-s try it with select not poll (for debugging)\n"
	"\t-h this help (and version number)\n"
	"\t-D just test out the hexdump display format (for debug)\n"
	);
	exit(1);

} /* END USAGE */



/******************
	MAIN
******************/
int
main(int argc, char ** argv)
{
	int rv = 0;
	int sel = 0;
	int c;
	struct fdstruct ** p = NULL;

	memset(&gfds, 0, sizeof(gfds)); /* clear the array, must be 0 terminated! */

	if(argc < 2){
		usage();
	}

	/* opts and such */
	while( (c= getopt(argc, argv, "phd:a:b:sD")) != -1) {
		switch(c){
			case 'd': /* first thing i want to know about */
				debug = atoi(optarg);
				break;
			case 'D': /* up top so it doesn't bother checking other opts */
				/* TODO: eventually let the user choose */
				dumpTest(0);
				exit(0);
				break;
			case 'p': /* order significant, before parseDevice opens the pty */
				ptmx = 1;
				break;
			case 'a':
				NULLCALL(gfds[0] = parseDevice(optarg));
				break;
			case 'b':
				NULLCALL(gfds[1] = parseDevice(optarg));
				break;
			case 's':
				sel= 1;
				break;
			case 'h':
			default:
				usage();
				break;
		} /* end switch */
	} /* end while */
	
	/* XXX redundant? */
	for(p = gfds; *p != NULL; p++){
		if( (*p)->fd < 0){
			usage();
		}
	}

	/* oh, why not */
	signalSetup();

	/* and now the loop de loop 
	 * TODO: be ready for multi-fd's: not twoWay but an array instead */
	rv = sel ? twoWaySelect(gfds) : twoWayPoll(gfds); 
	
	/* tidy up */
	/* TODO: move this into signal handler? */
	for(p = gfds; *p != NULL; p++){
		SYSCALL(close((*p)->fd));
		free((*p)->name);
		free(*p);
	}

	return rv;
}/* END MAIN */



/* EOF */

