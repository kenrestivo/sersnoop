/* $Id$ 
	serttys.h
	serial control funcs
*/

#ifndef SERTTYS_H
#define SERTTYS_H

int 
changespeed( int fd, int baud);

int
opentty(char *path, int baud);


#endif /* SERTTYS_H */

/* EOF */
