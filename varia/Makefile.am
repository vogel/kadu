
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/"	
	$(INSTALL_DATA) msg.wav "$(DESTDIR)/$(datadir)/apps/kadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/icons/"
	$(INSTALL_DATA) images/*.png "$(DESTDIR)/$(datadir)/apps/kadu/icons/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/1/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/*.gif "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/1/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/emots.txt "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/1/"

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/2/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.gif "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.GIF "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/emots.txt "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/2/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/msg.wav"
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/apps/kadu/themes/emoticons/gadu-gadu/2"/*
