# $Id: Makefile,v 1.2 2002/07/21 11:22:12 chilek Exp $

prefix = /usr
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

# increased with ABI changes
VERSION_MAJOR=0
# increased with API changes
VERSION_MINOR=10

CC = gcc 
CFLAGS = -g -O2 -ggdb -I.. -fPIC -Wall
LIBS = 

MAKE = /usr/bin/gmake
AR = /usr/bin/ar
INSTALL = /usr/bin/install -c
RANLIB = ranlib

OBJS = common.o libgadu.o search.o pubdir.o http.o dcc.o userlist.o events.o

SHARED = libgadu.so.$(VERSION_MAJOR).$(VERSION_MINOR)
STATIC = libgadu.a

all:	static shared

#

clean:
	rm -f *.o *.a *.so *.so.* *~ core

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
	$(CC) -g -shared -Wl,-soname=libgadu.so.$(VERSION_MAJOR) -o $(SHARED) $(OBJS) $(LIBS) -lc

#

install: install-shared install-static

install-common:
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL) -m 644 libgadu.h $(DESTDIR)$(includedir)
	$(INSTALL) -d $(DESTDIR)$(libdir)

install-shared:	install-common
	$(INSTALL) -m 755 $(SHARED) $(DESTDIR)$(libdir)

install-static:	install-common
	$(INSTALL) -m 755 $(STATIC) $(DESTDIR)$(libdir)
	
