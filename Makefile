#$Id$
#makefile for serial snooper

# Copyright (C) 2002  ken restivo <ken@restivo.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.                                  
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#names
PACKAGE=sersnoop
SRCDIR=sersnoop
REL=0.4

#dirs
DISTDIR=/mnt/www/restivo/projects/$(PACKAGE)/src
KENINCL=/mnt/kens/ki/is/c/kenincl

#build stuff
TARGETS= sersnoop
CFLAGS:= -g -Wall  -O3  -I/lib/modules/`uname -r`/build/include -I$(KENINCL) -DREL=\"$(REL)\"
sersnoop_OBJS:= main.o \
				pollcat.o \
				serttys.o \
				serptys.o \
				sig.o \
				selectloop.o \
				display.o \
				sock.o \
				util.o \
				common.o

sersnoop_HEADERS:= 	pollcat.h \
					serttys.h \
					serptys.h \
					sig.h \
					selectloop.h \
					display.h \
					sock.h \
					util.h \
					common.h \
					Makefile

#a gnuism TODO: replace this with proper autoconf shit
ifdef SSL
	ssl_OBJS:=sslsock.o
	SSLCFLAGS:=-DUSE_SSL
	SSLLDFLAGS:=-lssl -lcrypto
endif

# so i can get ssl cflags in there

%.o:%.c
	$(CC) $(CFLAGS) $(SSLCFLAGS) -c $< 

# 


all: $(TARGETS)

clean::
	rm -f $(TARGETS) $(sersnoop_OBJS) $(ssl_OBJS) core

sersnoop: $(sersnoop_OBJS) $(ssl_OBJS) $(sersnoop_HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SSLLDFLAGS) -o  $@ $(sersnoop_OBJS) $(ssl_OBJS)

#test targets

test: sersnoop
	./sersnoop -a/dev/ttyS1:38400 -b pty

safetest: sersnoop
	./sersnoop  -a/dev/ttyS0:9600 -b pty

netsafe: sersnoop
	./sersnoop  -a pty -b ts:23

nettest: sersnoop
	./sersnoop  -a pty -b ts:10010

servtest: sersnoop
	./sersnoop -a ts:23 -b 4242

servtty: sersnoop
	./sersnoop -a /dev/ttyS0:9600 -b 4242

dumptest: sersnoop
	./sersnoop -d 2 -D 

selecttest: sersnoop
	./sersnoop -s -a/dev/ttyS1:38400 -b pty

#debug and utility targets

debug: sersnoop
	@gdb ./sersnoop

dist: sersnoop
	set -e; \
	cd ..; \
	tar -cvf $(PACKAGE)-$(REL).tar \
			$(SRCDIR)/*.[ch] \
			$(SRCDIR)/Makefile \
			$(SRCDIR)/TO*  \
			$(SRCDIR)/README  \
	; gzip -f9  $(PACKAGE)-$(REL).tar

distinst: dist
	mv ../$(PACKAGE)-$(REL).tar.gz $(DISTDIR)
	(cd $(DISTDIR) && \
			rm -f $(PACKAGE)-current.tar.gz  ; \
			ln -s $(PACKAGE)-$(REL).tar.gz $(PACKAGE)-current.tar.gz)
	cp $(KENINCL)/kenmacros.h $(DISTDIR)/


#EOF

