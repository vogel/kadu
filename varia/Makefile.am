
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/"	
	$(INSTALL_DATA) msg.wav "$(DESTDIR)/$(kde_datadir)/kadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/kadubis/"	
	$(INSTALL_DATA) themes/emoticons/kadubis/*.gif "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/kadubis/"
	$(INSTALL_DATA) themes/emoticons/kadubis/emoticons_regexp "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/kadubis/"
	$(INSTALL_DATA) themes/emoticons/kadubis/emoticons_selector "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/kadubis/"
	$(mkinstalldirs) "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/*.gif "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/emoticons_regexp "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/original_kadu/"
	$(INSTALL_DATA) themes/emoticons/original_kadu/emoticons_selector "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/original_kadu/"

uninstall-local:
	rm -f "$(DESTDIR)/$(kde_datadir)/kadu/msg.wav"
	rm -f "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/kadubis"/*
	rm -f "$(DESTDIR)/$(kde_datadir)/kadu/themes/emoticons/original_kadu"/*
