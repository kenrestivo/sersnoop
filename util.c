/* $Id$ 
	util.c
	random shit
*/

#include <stdio.h>
#include <string.h>
#include <kenmacros.h>

/******************
	SPLITCOLON
	grabs the text before the colon and the number after it
	returns the num, slams name into name with null termination
	malloc's it. you must free.
******************/
int
splitColon(char * path, char ** name)
{
	char * colon = NULL;

	DPRINTF(1, "splitColon(): parsing out %s\n", path);
	if( (colon = strchr(path, ':')) == NULL){
		fprintf(stderr, "splitColon(): device must be in form 'string:number'\n");
		return(-1);
	}	

	if( (*name = strndup(path,colon - path )) == NULL ){
		fprintf(stderr, "splitColon(): strndup is out of memory\n");
		return(-1);
	}
	DPRINTF(1, "splitColon(): string portion is %s\n", *name);
	return (atoi(colon + 1));
}/* END SPLITCOLON */

/* EOF */
