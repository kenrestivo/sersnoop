/* $Id$ 
	display.c
	handles displaying what was recieved
	would like to make this an xxd-like format
	read/rip its code?
*/


#include <stdio.h>
#include <ctype.h>
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
display(char * source, char * buf, int len)
{
	char * clean = NULL;

	NULLCALL(clean =(char *)malloc(len));

	stripUnreadables(buf, clean, len);

	printf("%s: (%d) <%.*s>\n", source, len,  len, clean);

	return 0;

}/* END DISPLAY */




/* EOF */

