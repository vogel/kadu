
INCLUDES = -I../kadu $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GSM_INCLUDES) $(GADU_INCLUDES)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT \
-DBINDIR="\"$(bindir)\"" -DDATADIR="\"$(datadir)\""

CLEANFILES = *.so *.o *_moc.cpp

modules = $(patsubst %.cpp,%.so,$(wildcard *.cpp))

all: $(modules)

%.so: %.o %_moc.o
	if test -f $@.flags; then \
		source $@.flags; \
		echo MODULE_LDFLAGS=$$MODULE_LDFLAGS; \
	fi; \
	gcc -shared -o $@ $^ $$MODULE_LDFLAGS

%_moc.o: %_moc.cpp
	gcc -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS)

%.o: %.cpp %.h
	gcc -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS)

%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<

install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/modules/"	
	$(INSTALL_DATA) *.so "$(DESTDIR)/$(datadir)/kadu/modules/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/kadu/modules/*.so"
