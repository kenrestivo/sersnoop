/* $Id$ 
	serptys.c
	pty control stuff

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
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <kenmacros.h>

/* DEFS */
#define SLAVELEN 32

/*****************************
    LINUXGETPTY
	gets a sysv-style ptmx
	XXX this appears to be VERY linux-specific and non portable
	it also requires devpts fs in kernel and in in /etc/fstab
	puts the name of the slave term into *slaveName
	returns the fd or -1 if error
******************************/
#ifdef TIOCGPTN
static int 
linuxGetPty(void)
{
    int masterFd = 0 ;
	int slaveNum = -1;
	char * slaveName = NULL;
		
	/* first open a master pty (/dev/ptmx) */
	RETCALL( masterFd = open("/dev/ptmx", O_RDWR)) ;
		
	/* get the number of the slave - ptsname */
	RETCALL( ioctl(masterFd, TIOCGPTN, &slaveNum) );


	NULLCALL(slaveName = (char *)malloc(SLAVELEN));
    snprintf(slaveName, SLAVELEN, "/dev/pts/%d", slaveNum);
	
    DPRINTF(1, "linuxGetPty(): your pty is %s and the master is on fd %d\n",
        slaveName, masterFd);

	free(slaveName);

    return  masterFd;
} /* END LINUXGETPTY */
#endif

/**************************
    BSDGETPTY
    takes in: string for term
    returns: fd of master, if good, -1 if no go, -2 if busy
        name of slave pty to open, in input 
    note: this is portable, works with any bsd, linux, or solaris system
***************************/
static int 
bsdGetPty(void)
{
    /* TODO: is there any way to avoid overfloating the input string? 
        if it isn't 32 chars long? */
    char ltrs[] = "pqrs";
    char nums[] = "0123456789abcdef";
    char * ltrp;
    char * nump = NULL;
    char masterName[32] = "";
    char * slaveName = NULL;
    int masterFd = 0 ;

    /* get master */
    for(ltrp = ltrs; *ltrp != '\0'; ltrp++){
        for(nump = nums; *nump != '\0'; nump++){
            sprintf(masterName, "/dev/pty%c%c", *ltrp, *nump);
            if ( (masterFd = open(masterName, O_RDWR) ) >= 0){
                goto out; /* i hate gotos */
            }
            DPRINTF(1, "bsdGetTerm(): open %d %s: %s\n",
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
	NULLCALL(slaveName = (char *)malloc(SLAVELEN));
    snprintf(slaveName, SLAVELEN, "/dev/tty%c%c", *ltrp, *nump);

    DPRINTF(1, "bsdGetPty(): your pty is %s and the master is %s open on fd %d\n",
        slaveName, ttyname(masterFd), masterFd);

	free(slaveName);

    return masterFd;

} /* END BSDGETPTY */

/**************************
    GETPTY
    takes in: string for term
    returns: fd of master, if good, -1 if no go, -2 if busy
        name of slave pty to open, in input 
	 TODO: put a proper configure.in test and choose based on that 
	NOTE: all of these functions malloc *slaveName, you must free
***************************/
int 
getPty(int ptmx)
{

	if(ptmx){
#ifdef TIOCGPTN
		return(linuxGetPty());	
#else
	DPRINTF(1, "getPty(): can't use ptmx on this system\n");
	return(-1);
#endif 
	} else {
		return(bsdGetPty());
	}

} /* END GETPTY */

/* EOF */

