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

/* GLOBS */
int debug = 1;


/******************
	MAIN
******************/
int
main(int argc, char ** argv)
{
	unsigned int slaveNum = -1;
	int ptfd = -1;	
	int ttyfd = -1;	

	/* open the local pty */
	SYSCALL(ptfd = linuxGetPty(&slaveNum));	

	/* open the serial port */
	ttyfd = opentty("/dev/ttyS1", B38400);
	
	/* eventually move the cleanups out, to be signal-clean */
	SYSCALL(close(ptfd));
	SYSCALL(close(ttyfd));
	return 0;
}/* END MAIN */



/* EOF */

