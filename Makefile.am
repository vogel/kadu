
SUBDIRS = @MY_SUBDIRS@

DIST_SUBDIRS = @MY_SUBDIRS@

EXTRA_DIST = admin kadu.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO kadu.lsm kadu.spec 

AUTOMAKE_OPTIONS = foreign 


###########################################################################
# Precompiled headers support
###########################################################################

# dirty hack - leading space required because od automake "if" support
# time to say automake goodbye?
 ifeq ($(enable_pheaders),yes)

all: kadu-headers

kadu-headers:
	@echo
	@echo -e "\033[32m>>>\033[0m Precompiling headers ..."
	@echo
	$(MAKE) -C kadu kadu-headers.h.gch
	@echo
	@echo -e "\033[32m>>>\033[0m Precompiled headers ready."
	@echo

 endif

###########################################################################

install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(includedir)/kadu/"
	$(INSTALL_DATA) kadu-config.h "$(DESTDIR)/$(includedir)/kadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/"	
	$(INSTALL_DATA) AUTHORS "$(DESTDIR)/$(datadir)/kadu/"
	$(INSTALL_DATA) ChangeLog "$(DESTDIR)/$(datadir)/kadu/"
	$(INSTALL_DATA) COPYING "$(DESTDIR)/$(datadir)/kadu/"
	$(INSTALL_DATA) HISTORY "$(DESTDIR)/$(datadir)/kadu/"
	$(INSTALL_DATA) README "$(DESTDIR)/$(datadir)/kadu/"
	$(INSTALL_DATA) THANKS "$(DESTDIR)/$(datadir)/kadu/"
	@echo
	@echo " **************************************"
	@echo " * Kadu is now successfully installed *"
	@echo " **************************************"
	@echo
	@echo " Thanks for choosing Kadu Communicator and ENJOY!!!"
	@echo
	@echo "                                  Kadu Team"
	@echo

uninstall-local:
	rm -f "$(DESTDIR)/$(includedir)/kadu/kadu-config.h"
	rm -f "$(DESTDIR)/$(datadir)/kadu/*"
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2"/*
