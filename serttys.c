/* $Id$ 
	ttys.c
	all the tty control functions
*/

#include <fcntl.h>
#include <strings.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <kenmacros.h>
#include "util.h"

/* DEFS */

/******************
	DECODEBAUD
	takes in a reg'lar number and returns the weirdo Bxxx code needed for changespeed
******************/
static int
decodeBaud(int baud)
{
	struct ipair { 
		int weird;
		int real; 
	};
	struct ipair 
	baudy[] = {
		{B0,0 },
		{B50, 50 },
		{B75,75 },
		{B110,110 },
		{B134, 134 },
		{B150,150 },
		{B200, 200},
		{B300,300 },
		{B600, 600 },
		{B1200, 1200 },
		{B1800, 1800 },
		{B2400,2400 },
		{B4800,4800 },
		{B9600, 9600},
		{B19200, 19200 },
		{B38400, 38400 },
		{B57600, 57600 },
		{B115200, 115200 },
		{B230400, 230400 },
		{B460800, 460800 }
	};
	int i;

	for(i=0; i< STAT_ARRAY_SIZE(baudy); i++){
		if(baudy[i].real == baud){
			DPRINTF(1, "decodeBaud(): found human baud %d is weird code 0x%X\n", 
				baud, baudy[i].weird);
			return(baudy[i].weird);
		}
	}

	DPRINTF(1, "decodeBaud(): human baud %d not found\n", baud);
	return(-1);

}/* END DECODEBAUD */


/******************
	CHANGESPEED
	NOTE! baud is not a speed int, it's one of the termios.h Bxxx constants!
******************/
static int 
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

	/* TODO: cfmakeraw() instead of all this shit. 
		i still need to set baud tho */

	/* discard anything sitting in ingoing our outgoing buffers */
	RETCALL(tcflush(fd,TCIOFLUSH));

	return(0);
}/* END CHANGESPEED */


/******************
	OPENTTY
	opens up a tty in raw mode
******************/
int
opentty(char *path )
{
	int fd;
	char * ttyname = NULL;
	int humanbaud = 0 ;
	int baudcode = 0 ;

	/* split out the options */
	RETCALL( (humanbaud = splitColon(path, &ttyname)) );

	/* decode baud */
	RETCALL(baudcode = decodeBaud(humanbaud));
	

	DPRINTF(1, "opentty(): opening %s with baudcode 0x%X\n", ttyname, baudcode);

	/* XXX weird... i'm using select/poll, should i use NDELAY?? */
	RETCALL(fd = open(ttyname, O_RDWR | O_NDELAY ) ) ;

	RETCALL(changespeed(fd, baudcode) );

	DPRINTF(1, "opentty(): opened %s as fd %d\n", ttyname, fd);

	free(ttyname);

	return(fd);
}/* END OPENTTY  */

/* EOF */

