/* $Id$ 
	display.c
	handles displaying what was recieved
	would like to make this an xxd-like format
	read/rip its code?
*/


#include <stdio.h>

/******************
	DISPLAY
	shows the buffer in an xxd-like format
******************/
int
display(char * source, char * buf, int len)
{
	printf("%s: (%d) <%s>\n", source, len, buf);

	return 0;

}/* END DISPLAY */




/* EOF */

