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

	DPRINTF(1, "opening %s at %d baud\n", path, baud);

	RETCALL(fd = open(path, O_RDWR | O_NDELAY ) ) ;
	if(baud > 0){
		RETCALL(changespeed(fd, baud) );
	}

	DPRINTF(1, "opened %s as fd %d\n", path, fd);

	return(fd);
}/* END OPENTTY  */

/* EOF */

