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
    meth=SSLv3_method();
    ctx=SSL_CTX_new(meth);

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_file(ctx,keyfile,SSL_FILETYPE_PEM)))
      sslPerror("Couldn't read certificate file");

	/* XXX this is bullshit. i'm not using private keys */
    SSL_CTX_set_default_passwd_cb(ctx,password_cb);

    if(!(SSL_CTX_use_PrivateKey_file(ctx,keyfile,SSL_FILETYPE_PEM)))
      sslPerror("Couldn't read key file");

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,calist,0)))
      sslPerror("Couldn't read CA list");
    SSL_CTX_set_verify_depth(ctx,1);

    /* XXX use /dev/random instead? Load randomness */
    if(!(RAND_load_file(RND,1024*1024)))
      sslPerror("Couldn't load randomness");

    return ctx;

}/* END NEWCTX */



/******************
	OPENSSLSOCK
******************/
struct fdstruct *
openSSLSock (char * path)
{
	struct fdstruct * lfd = NULL;

	NULLCALL(lfd = (struct fdstruct *)malloc(sizeof(struct fdstruct)));

	lfd = openSock(path);

	if(!ctx){
		ctx = newCTX(); 
	}

	lfd->ssl = SSL_new(ctx);

    sbio=BIO_new_socket(lfd->fd,BIO_NOCLOSE);
    SSL_set_bio(ssl,sbio,sbio); /* XXX um, do i want/need this? */
    if(SSL_connect(lfd->ssl)<=0)
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

