/*	$Id$ 
	ssl socket i/o functions

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

#include <stdlib.h>
#include <openssl/ssl.h>
#include <kenmacros.h>
#include "common.h"
#include "sock.h"
#include "sslsock.h"

#define RND "/dev/urandom"

extern BIO * bio_err;

SSL_CTX * ctx;



/******************
	SSLPERROR
******************/
sslPerror(char *  str)
{
    BIO_printf(bio_err,"%s\n",str);
    ERR_print_errors(bio_err);
	exit(1); /* XXX do i have a more elegant way of handling exceptions? */

}/* END  SSLPERROR */


/* TODO: write sigpipe handler, that reports the premature close, nonfatal */

/* TODO: close function SSL_shutdown() */


/******************
	NEWCTX
******************/
SSL_CTX *
newCTX(void)
{
   SSL_METHOD *meth;
    SSL_CTX *ctx;

    /* bio_err is set iff the initialization has already been done */
    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();

      /* TODO change this to a global logfile or syslog  */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler, to deal with a bug with FIN's */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    meth=SSLv23_method();
    ctx=SSL_CTX_new(meth);

	/* NOTE: this will only work on systems that have a /dev/urandom */

    return ctx;

}/* END NEWCTX */



/******************
	OPENSSLSOCK
******************/
struct fdstruct *
openSSLSock (char * path)
{
	struct fdstruct * lfd = NULL;

	/* remember, this malloc's the struct lfd, and returns it */
	lfd = openSock(path);

	if(!ctx){
		ctx = newCTX(); 
	}

	lfd->ssl = SSL_new(ctx);
    if(SSL_set_fd(lfd->ssl, lfd->fd) < 1)
      berr_exit("main: SSL set fd error");
	

	/* XXX i'm pretty sure i need the standard ssl connect_fd here */
    if(SSL_connect(lfd->ssl) <= 0)
      berr_exit("main: SSL connect error");
    check_cert_chain(lfd->ssl,lfd->name /*XXX grab the hostname portion */ );

	return lfd;

}/* END OPENSSLSOCK  */


/******************
	OPENSSLSERVER
******************/
struct fdstruct *
openSSLServer(char * path)
{
	struct fdstruct * lfd = NULL;

	NULLCALL(lfd = (struct fdstruct *)malloc(sizeof(struct fdstruct)));

	lfd = openServer(path);

	return lfd;

}/* END OPENSSLSERVER */

/* EOF */

