
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/"	
	$(INSTALL_DATA) msg.wav "$(DESTDIR)/$(datadir)/apps/kadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/icons/"
	$(INSTALL_DATA) images/*.png "$(DESTDIR)/$(datadir)/apps/kadu/icons/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/kadubis/"	
	$(INSTALL_DATA) themes/emoticons/kadubis/*.gif "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/kadubis/"
	$(INSTALL_DATA) themes/emoticons/kadubis/emoticons_regexp "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/kadubis/"
	$(INSTALL_DATA) themes/emoticons/kadubis/emoticons_selector "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/kadubis/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/*.gif "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/emoticons_regexp "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/emoticons_selector "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/original_kadu/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/msg.wav"
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/kadubis"/*
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/original_kadu"/*
