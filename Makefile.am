
SUBDIRS = @MY_SUBDIRS@

DIST_SUBDIRS = @MY_SUBDIRS@

EXTRA_DIST = admin kadu.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO kadu.lsm kadu.spec 

AUTOMAKE_OPTIONS = foreign 

install-data-local:
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
	rm -f "$(DESTDIR)/$(datadir)/kadu/*"
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2"/*
