
install-data-local:

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default/"
	$(INSTALL_DATA) themes/sounds/default/*.wav "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"
	$(INSTALL_DATA) themes/sounds/default/sound.conf "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/icons/default/"
	$(INSTALL_DATA) themes/icons/default/*.png "$(DESTDIR)/$(datadir)/kadu/themes/icons/default"
	$(INSTALL_DATA) themes/icons/default/icons.conf "$(DESTDIR)/$(datadir)/kadu/themes/icons/default"

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/*.gif "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/emots.txt "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.gif "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.GIF "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/emots.txt "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"

	$(mkinstalldirs) "$(DESTDIR)/$(bindir)/"	
	$(INSTALL_SCRIPT) scripts/kadu-mozilla "$(DESTDIR)/$(bindir)/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/icons/default"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2"/*
	rm -f "$(DESTDIR)/$(bindir)/kadu-mozilla"
