/* $Id$ 
	serialmon.c
	does raw tty comms, forwards to/from pty, and prints out waz happenin.
*/

#include <fcntl.h>
#include <sys/ioctl.h>
#include <kenmacros.h>

/* DEFS */
#define MAXSLAVE 1024

/* GLOBS */
int debug = 1;

/*****************************
    LINUXGETPTY
	gets a sysv-style ptmx
	returns the fd or -1 if error
	also puts the pty number into slaveNump
******************************/
int 
linuxGetPty(unsigned int * slaveNump){
    int masterFd = 0 ;
        
    /* first open a master pty (/dev/ptmx) */
    RETCALL( masterFd = open("/dev/ptmx", O_RDWR)) ;
        
    /* change ownerships - grantpt */
    /* RETCALL ( ioctl(masterFd, TIOCGRANT) ); */
    
    
    /* unlock it - unlockpt */
        /* is this a no-op on linux?? */
    
    /* get the name of the slave - ptsname */
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

	SYSCALL(ptfd = linuxGetPty(&slaveNum));	
	
	SYSCALL(close(ptfd));
	return 0;
}/* END MAIN */



/* EOF */

