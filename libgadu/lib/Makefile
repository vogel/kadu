# $Id: Makefile,v 1.1 2002/07/09 22:22:02 chilek Exp $

CC = gcc 
CFLAGS = -g -O2 -ggdb -I.. -fPIC -Wall
LIBS = 

MAKE = /usr/bin/gmake
AR = /usr/bin/ar
INSTALL = /usr/bin/install -c
RANLIB = ranlib

prefix = /usr
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

OBJS = common.o libgadu.o search.o pubdir.o http.o dcc.o userlist.o events.o

SHARED = libgadu.so.0.9.0
STATIC = libgadu.a

all:	static shared

#

clean:
	rm -f *.o *~ core $(STATIC) $(SHARED)

distclean:	clean
	rm -f Makefile

#

static: $(STATIC)

$(STATIC): $(OBJS)
	$(AR) r $(STATIC) $(OBJS) 
	$(RANLIB) $(STATIC)

#

shared:	$(SHARED)

$(SHARED): $(OBJS)
	$(CC) -g -shared -Wl,-soname,libgadu.so -o $(SHARED) $(OBJS) $(LIBS) -lc

#

install: install-shared install-static

install-shared:
	$(INSTALL) -d $(libdir)
	$(INSTALL) -m 755 $(SHARED) $(libdir)

install-static:
	$(INSTALL) -d $(libdir)
	$(INSTALL) -m 755 $(STATIC) $(libdir)
	
