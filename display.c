/* $Id$ 
	display.c
	handles displaying what was recieved
	would like to make this an xxd-like format
	read/rip its code?
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

