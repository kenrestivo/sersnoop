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

#dirs
KENINCL=/mnt/kens/ki/is/c/kenincl

#build stuff
TARGETS= sersnoop
CFLAGS:= -g -Wall  -O3  -I/lib/modules/`uname -r`/build/include -I$(KENINCL)
sersnoop_OBJS:= sersnoop.o pollcat.o serttys.o serptys.o sig.o selectloop.o

#


all: $(TARGETS)

clean::
	rm -f $(TARGETS) $(sersnoop_OBJS) core

sersnoop: $(sersnoop_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o  $@ $(sersnoop_OBJS)

test: sersnoop
	./sersnoop

selecttest: sersnoop
	./sersnoop -s

debug: sersnoop
	@gdb ./sersnoop


#EOF

