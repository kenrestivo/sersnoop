/* $Id$ 
	commmon.h
	structs used globally by serialsnoop

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

#ifndef COMMON_H
#define COMMON_H

#ifdef USE_SSL
#include <openssl/ssl.h>
#endif /* USE_SSL */

/* TYPES */
struct fdstruct {
		int fd; /* the fd that has been open */
		char * name; /* for the display function to use */
#ifdef USE_SSL
		SSL * ssl; /* the ssl object associated with this connection */
#endif /* USE_SSL */
};

/* FUNCS */

char * 
lookupName( int fd);

#endif /* COMMON_H */

/* EOF */

