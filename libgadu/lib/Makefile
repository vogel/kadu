# $Id: Makefile,v 1.3 2002/08/19 10:56:53 chilek Exp $

prefix = /usr
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

# increased with ABI changes
VERSION_MAJOR=2
# increased with API changes
VERSION_MINOR=0

CC = gcc 
CFLAGS = -g -O2 -ggdb -I.. -Wall
LIBS = 

MAKE = /usr/bin/gmake
AR = /usr/bin/ar
INSTALL = /usr/bin/install -c
RANLIB = ranlib
STRIP = @STRIP@
LN_S = @LN_S@

OBJS = common.o libgadu.o search.o pubdir.o http.o dcc.o userlist.o events.o
LOBJS = common.lo libgadu.lo search.lo pubdir.lo http.lo dcc.lo userlist.lo events.lo

SHAREDMM = libgadu.so
SHAREDM = $(SHAREDMM).$(VERSION_MAJOR)
SHARED = $(SHAREDM).$(VERSION_MINOR)
STATIC = libgadu.a

all:	static shared

#

clean:
	rm -f *.o *.lo *.a *.so *.so.* *~ core

distclean:	clean
	rm -f Makefile

#

static: $(STATIC)

$(STATIC): $(OBJS)
	$(AR) r $(STATIC) $(OBJS) 
	$(RANLIB) $(STATIC)

#

%.lo:	%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -fPIC $< -o $@

shared:	$(SHARED)

$(SHARED): $(LOBJS)
	$(CC) -g -shared -Wl,-soname=libgadu.so.$(VERSION_MAJOR) -o $(SHARED) $(LOBJS) $(LIBS) -lc
	$(LN_S) -f $(SHARED) $(SHAREDM)
	$(LN_S) -f $(SHARED) $(SHAREDMM)

#

install: install-shared install-static

install-common:
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL) -m 644 libgadu.h $(DESTDIR)$(includedir)
	$(INSTALL) -d $(DESTDIR)$(libdir)

install-shared:	install-common @strip_libgadu_so@
	$(INSTALL) -m 755 $(SHARED) $(DESTDIR)$(libdir)
	cd $(DESTDIR)$(libdir) && $(LN_S) -f $(SHARED) $(SHAREDM)
	cd $(DESTDIR)$(libdir) && $(LN_S) -f $(SHARED) $(SHAREDMM)

install-static:	install-common
	$(INSTALL) -m 755 $(STATIC) $(DESTDIR)$(libdir)

strip-libgadu-so:
	$(STRIP) $(SHARED)

