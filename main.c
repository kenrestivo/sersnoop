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

/* DEFS */
#define MAXSLAVE 1024

/* GLOBS */
int debug = 1;

/*****************************
    LINUXGETPTY
	gets a sysv-style ptmx
	XXX this appears to be VERY linux-specific and non portable
	it also requires devpts fs in kernel and in in /etc/fstab
	returns the fd or -1 if error
	also puts the pty number into slaveNump
******************************/
int 
linuxGetPty(unsigned int * slaveNump){
    int masterFd = 0 ;
		
	/* first open a master pty (/dev/ptmx) */
	RETCALL( masterFd = open("/dev/ptmx", O_RDWR)) ;
		
	/* get the number of the slave - ptsname */
	RETCALL( ioctl(masterFd, TIOCGPTN, slaveNump) );


   DPRINTF( 1, "your pty is /dev/pts/%d and the master is on fd %d  \n",
        *slaveNump,  masterFd);

    return  masterFd;
 } /* END LINUXGETPTY */



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

