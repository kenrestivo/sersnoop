/* $Id$ 
	util.c
	random shit

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

#define _GNU_SOURCE
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
	char * mangle = NULL;

	/* bsd wants it this way. no strndup here dude */
	if( (mangle = strdup(path)) == NULL ){
		fprintf(stderr, "splitColon(): strdup is out of memory\n");
		return(-1);
	}

	DPRINTF(1, "splitColon(): parsing out %s\n", path);
	if( (colon = strchr(mangle, ':')) == NULL){
		fprintf(stderr, "splitColon(): device must be in form 'string:number'\n");
		return(-1);
	}	

	/* chomp it */
	*colon = '\0';

	DPRINTF(1, "splitColon(): string portion is %s\n", mangle);

	/* return values, name gets returned in args, port as retval */
	*name = mangle;
	return (atoi(colon + 1));

}/* END SPLITCOLON */

/* EOF */
