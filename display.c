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

	printf("%d:%d %s: (%d) <%.*s>\n", 
		tv.tv_sec, tv.tv_usec, ttyname(sourcefd), len,  len, clean);

	total += len;


	return 0;

}/* END DISPLAY */




/* EOF */

