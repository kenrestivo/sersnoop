/* $Id$ 
	serptys.h
	pty functions
*/

#ifndef SERPTYS_H
#define SERPTYS_H


/* NOTE: this malloc's *slaveName, you must free */
int 
getPty(int ptmx);

#endif /* SERPTYS_H */

/* EOF */

