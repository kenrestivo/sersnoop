/* $Id$ 
	serptys.h
	pty functions
*/

#ifndef SERPTYS_H
#define SERPTYS_H


int 
linuxGetPty(unsigned int * slaveNump);

int 
bsdGetPty(char ** slaveName, int slavelen);

#endif /* SERPTYS_H */

/* EOF */

