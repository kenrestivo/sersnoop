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

/* DEFS */
#define MAXSLAVE 1024

/* GLOBS */
int debug = 1;

/*****************************
    LINUXGETPTY
	gets a sysv-style ptmx
	XXX this appears to be VERY linux-specific and non portable
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
	CHANGESPEED
	NOTE! baud is not a speed int, it's one of the termios.h Bxxx constants!
******************/
int 
changespeed( int fd, int baud)
{
	/* termios interface */
	struct termios tio;

	RETCALL(tcgetattr(fd, &tio) );

	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL  ; /* 8bit non parity stop 1 */
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5 ;
	cfsetispeed(&tio, baud);
	cfsetospeed(&tio, baud);
	RETCALL (tcsetattr(fd, TCSANOW, &tio) ) ;

	/* drain all the crap */
	tcflush(fd,TCIOFLUSH);

	return(0);
}/* END CHANGESPEED */


/******************
	OPENTTY
	opens up a tty in raw mode
******************/
int
opentty(char *path, int baud)
{
	int fd;

	RETCALL(fd = open(path, O_RDWR | O_NDELAY ) ) ;
	if(baud > 0){
		RETCALL(changespeed(fd, baud) );
	}

	DPRINTF(1, "opened %s as fd %d\n", path, fd);

	return(fd);
}/* END OPENTTY  */



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

