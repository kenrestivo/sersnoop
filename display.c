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
	HEXDUMP
	like the man says, dump the hex ;-)
******************/
static void
hexDump(unsigned char * buf, int len)
{
	char posfmt[]= "\n    0x%0.4x: "; /* maximum 65535 before rolling ouver */
	char bytefmt[] = "%0.2x%0.2x ";
	int bytesline = 0; /* position on the screen */
	int pos = 0; /* position in the buffer */
	char * asciistart = NULL; /* a pointer because index 0 is problematic */
	char cleanbuf[MAXBYTESLINE];
	
	while(pos < len){
		if(bytesline == 0 || bytesline >= MAXBYTESLINE){
			/* print the header */
			printf(posfmt, pos);
			/* fmt has \n in it, so we are resetting the column = here. */
			bytesline = 0;
			asciistart = &buf[pos];
		}
	
		/* print 2 bytes */
		printf(bytefmt, buf[pos++], buf[pos++]);
		bytesline += 2;
		DPRINTF(2, "pos = %d, bytesline = %d\n", 
					pos, bytesline);

		/* XXX i fucking hate this hack. i hate state machines. but it works. */
		if(bytesline >= MAXBYTESLINE && asciistart != NULL){
			stripUnreadables(asciistart, cleanbuf, MAXBYTESLINE);
			printf("  %.*s", MAXBYTESLINE-1, cleanbuf);
			asciistart = NULL;
		}
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

	DPRINTF(1, "here is a test buffer displayed:\n");
	hexDump(testbuf0, sizeof(testbuf0));

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
		printf("\ttotal %d\n--------\n", total);
		lastsource = sourcefd;
		total = 0;
	}

	printf("%d:%d %s: 0x%0.4X (%d)\n", 
		(int)tv.tv_sec, (int)tv.tv_usec, 
		ttyname(sourcefd), len , len );

	hexDump(buf, len);

	total += len;


	return 0;

}/* END DISPLAY */




/* EOF */

