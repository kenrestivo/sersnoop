/* $Id$ 
	display.c
	handles displaying what was recieved
	would like to make this an xxd-like format

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
#include <ctype.h>
#include <sys/time.h>
#include <kenmacros.h>


#define MAXBYTESLINE  16


/******************
	STRIPUNREADABLES
******************/
static int
stripUnreadables(char * dirty, char * clean, int len)
{
	char * d = dirty;
	char * c = clean;

	while((d - dirty) < len){
		if(isprint(*d)){
			*c = *d;
		} else {
			*c = '.';
		}

		d++;
		c++;
	}

	return 0;

}/* END STRIPUNREADABLES */



/******************
	PRINTLINE
	prints a line of hex and ascii
	linebuf is just THE LINE (not the whole buf)
	len is the length of the line (so i can change it later)
	mainpos is the position in the main buffer of the first byte of this line
		does that make any sense?
	returns the new mainpos, or -1 if error
******************/
int
printLine(unsigned char * linebuf, int linelen, int mainpos)
{
	char posfmt[]= "\n    0x%0.4x: "; /* maximum 65535 before rolling over */
	char bytefmt[] = "%02x ";
	char * cleanbuf = NULL;
	int i = 0;

	/* the header */
	printf(posfmt, mainpos);

	/* the bytes */
	for(i = 0; i < linelen; i++){
		printf(bytefmt, linebuf[i]);
		DPRINTF(2, "mainpos = %d, bytesline = %d\n", 
					mainpos, i);
		mainpos++; /* for debug, mostly */
	}

	/* the ascii */
	NULLCALL(cleanbuf = (char *)malloc(linelen));
	stripUnreadables(linebuf, cleanbuf, linelen);
	/* XXX oops. with short packets, my column justtification is fucked */
	printf("  %.*s", linelen, cleanbuf);

	free(cleanbuf);

	return (mainpos); /* note! this has been incremented in the loop */
}/* END PRINTLINE */



/******************
	HEXDUMP
	like the man says, dump the hex ;-)
******************/
static void
hexDump(unsigned char * buf, int len)
{
	int pos = 0; /* position in the buffer */
	int linelen = MAXBYTESLINE;
	
	while(pos < len){
		/* XXX i have remainder issues here */
		pos = printLine(&buf[pos], linelen, pos);
	} /* end while */

	putchar('\n'); /* XXX redundant? */

}/* END HEXDUMP */



/******************
 * DUMPTEST
 * just exercises hexDump() 
 * ******************/
void
dumpTest(int which)
{
	char testbuf0[256] ;
	int i;

	for (i = 0; i< 256; i++){
		testbuf0[i] = i;
	}

	DPRINTF(1, "here is a test buffer 0 displayed:\n");
	hexDump(testbuf0, sizeof(testbuf0));

	DPRINTF(1, "here is the first 2 bytes of test buffer 0:\n");
	hexDump(testbuf0, 2);

}/* END DUMPTEST */




/******************
	DISPLAY
	shows the buffer in an xxd-like format
******************/
int
display(int sourcefd, char * buf, int len)
{
	static int lastsource = -1;
	static int total = 0;
	struct timeval tv;

	RETCALL(gettimeofday(&tv, NULL));

	if(lastsource != sourcefd){
		printf("\n    total 0x%04x (%d)\n--------\n", 
			total, total);
		lastsource = sourcefd;
		total = 0;
	}

	printf("%d:%d %s: 0x%04X (%d)\n", 
		(int)tv.tv_sec, (int)tv.tv_usec, 
		ttyname(sourcefd), len , len );

	hexDump(buf, len);

	total += len;


	return 0;

}/* END DISPLAY */




/* EOF */

