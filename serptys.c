/* $Id$ 
	serptys.c
	pty control stuff
*/


#include <fcntl.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <kenmacros.h>

/* DEFS */
#define SLAVELEN 12

/*****************************
    LINUXGETPTY
	gets a sysv-style ptmx
	XXX this appears to be VERY linux-specific and non portable
	it also requires devpts fs in kernel and in in /etc/fstab
	returns the fd or -1 if error
	also puts the pty number into slaveNump
******************************/
int 
linuxGetPty(unsigned int * slaveNump)
{
    int masterFd = 0 ;
		
	/* first open a master pty (/dev/ptmx) */
	RETCALL( masterFd = open("/dev/ptmx", O_RDWR)) ;
		
	/* get the number of the slave - ptsname */
	RETCALL( ioctl(masterFd, TIOCGPTN, slaveNump) );


   DPRINTF( 1, "your pty is /dev/pts/%d and the master is on fd %d  \n",
        *slaveNump,  masterFd);

    return  masterFd;
} /* END LINUXGETPTY */

/**************************
    BSDGETPTY
    takes in: string for term
    returns: fd of master, if good, -1 if no go, -2 if busy
        name of slave pty to open, in input 
    note: this is portable, works with any bsd, linux, or solaris system
***************************/
int 
bsdGetPty(char ** slaveName, int slavelen)
{
    /* TODO: is there any way to avoid overfloating the input string? 
        if it isn't 32 chars long? */
    char ltrs[] = "pqrs";
    char nums[] = "0123456789abcdef";
    char * ltrp;
    char * nump = NULL;
    char masterName[32] = "";
    int masterFd = 0 ;

    /* get master */
    for(ltrp = ltrs; *ltrp != '\0'; ltrp++){
        for(nump = nums; *nump != '\0'; nump++){
            sprintf(masterName, "/dev/pty%c%c", *ltrp, *nump);
            if ( (masterFd = open(masterName, O_RDWR) ) >= 0){
                goto out; /* i hate gotos */
            }
            DPRINTF(1, "bsdGetTerm: open %d %s: %s\n",
                masterFd, masterName, strerror(errno));
        }
    }
    out:
    /* oops, end of the line, no tty's */
    if(*ltrp == '\0' && *nump == '\0'){
        return -2;
    }

    /* if i'm still zero, or stuff is empty, something's wackie */
	if(debug> 0){
		assert(masterFd > 0);
		assert(*nump != '\0' && *ltrp != '\0');
	}

    /* all is well, return the tty name  and the master fd*/
	NULLCALL(*slaveName = (char *)malloc(SLAVELEN));
    snprintf(*slaveName, SLAVELEN, "/dev/tty%c%c", *ltrp, *nump);

    DPRINTF(1,
        "yee haw! your pty is %s and the master is %s open on %d\n",
        *slaveName, ttyname(masterFd), masterFd);

    return masterFd;

} /* END BSDGETPTY */



/* EOF */

