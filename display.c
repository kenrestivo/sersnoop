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


#define MAXCOL 15


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
	unsigned char * p = buf;

	if(len > MAXCOL){
		printf("\n\t\t");
	}

	while((p - buf) < len){
		printf("%02x ", *p);
		p++;
	}

	putchar('\n');

}/* END HEXDUMP */




/******************
	DISPLAY
	shows the buffer in an xxd-like format
******************/
int
display(int sourcefd, char * buf, int len)
{
	char * clean = NULL;
	static int lastsource = -1;
	static int total = 0;
	struct timeval tv;

	RETCALL(gettimeofday(&tv, NULL));

	NULLCALL(clean =(char *)malloc(len));

	stripUnreadables(buf, clean, len);

	if(lastsource != sourcefd){
		printf("\ttotal %d\n--------\n", total);
		lastsource = sourcefd;
		total = 0;
	}

	printf("%d:%d %s: (%d) <%.*s> ", 
		(int)tv.tv_sec, (int)tv.tv_usec, 
		ttyname(sourcefd), len,  len, clean);

	hexDump(buf, len);

	total += len;


	return 0;

}/* END DISPLAY */




/* EOF */

