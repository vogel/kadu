
SUBDIRS = kadu po varia

EXTRA_DIST = admin kadu.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO kadu.lsm kadu.spec 

AUTOMAKE_OPTIONS = foreign 

mostlyclean-local:
	cd libgadu && $(MAKE) mostlyclean

clean-local:
	cd libgadu && $(MAKE) clean

distclean-local:
	cd libgadu && $(MAKE) distclean

maintainer-clean-local:
	cd libgadu && $(MAKE) maintainer-clean
