/* $Id$ 
	serialmon.c
	does raw tty comms, forwards to/from pty, and prints out waz happenin.
*/

#include <fcntl.h>
#include <strings.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <kenmacros.h>

#include "pollcat.h"
#include "serttys.h"
#include "serptys.h"
#include "sig.h"

/* GLOBS */
int debug = 1;


/******************
	MAIN
******************/
int
main(int argc, char ** argv)
{
	unsigned int slaveNum = -1;
	char * slaveName = NULL ;
	int ptfd = -1;	
	int ttyfd = -1;	
	int rv = 0;

	/* oh, why not */
	signalSetup();

	/* open the local pty */
	/* XXX fucked 
		SYSCALL(ptfd = linuxGetPty(&slaveNum));	 */
	SYSCALL(ptfd = bsdGetPty(&slaveName, sizeof(slaveName)));

	/* open the serial port */
	ttyfd = opentty("/dev/ttyS1", B38400);

	/* and now the loop de loop */
	rv = twoWayPoll(ptfd, ttyfd); 
	
	/* eventually move the cleanups out, to be signal-clean */
	free(slaveName);
	SYSCALL(close(ptfd));
	SYSCALL(close(ttyfd));
	return rv;
}/* END MAIN */



/* EOF */

