# $Id: Makefile,v 1.1 2002/07/09 22:22:02 chilek Exp $

prefix = /usr
datadir = ${prefix}/share
mandir = ${prefix}/man

MAKE = /usr/bin/gmake
INSTALL = /usr/bin/install -c

#

all:	libgadu 

libgadu:
	cd lib && $(MAKE) all

ekg:	
	cd src && $(MAKE) all

examples:
	cd examples && $(MAKE) all

#

install:	all install-libgadu 

install-libgadu:
	cd lib && $(MAKE) install

install-ekg:	install-ekg-themes install-ekg-man
	$(INSTALL) -d $(prefix)/bin
	cd src && $(MAKE) install

install-ekg-themes:
	$(INSTALL) -d $(datadir)/ekg/themes
	$(INSTALL) -m 644 themes/*.theme $(datadir)/ekg/themes

install-ekg-man:
	$(INSTALL) -d $(mandir)/man1
	$(INSTALL) -m 644 docs/ekg.man.en $(mandir)/man1/ekg.1

#

clean:	clean-libgadu  clean-examples

clean-libgadu:
	cd lib && $(MAKE) clean

clean-ekg:
	cd src && $(MAKE) clean

clean-examples:
	cd examples && $(MAKE) clean

#

distclean:	distclean-libgadu  distclean-examples
	rm -f *~ config.log config.cache config.status config.h Makefile

distclean-libgadu:
	cd lib && $(MAKE) distclean

distclean-ekg:
	cd src && $(MAKE) distclean

distclean-examples:
	cd examples && $(MAKE) distclean


