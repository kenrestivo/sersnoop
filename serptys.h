/* $Id$ 
	serptys.h
	pty functions
*/

#ifndef SERPTYS_H
#define SERPTYS_H


/* NOTE: this malloc's *slaveName, you must free */
int 
getPty(char ** slaveName );

#endif /* SERPTYS_H */

/* EOF */

