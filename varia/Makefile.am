
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/"	
	$(INSTALL_DATA) msg.wav "$(DESTDIR)/$(datadir)/kadu/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/icons/"
	$(INSTALL_DATA) images/*.png "$(DESTDIR)/$(datadir)/kadu/icons/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/"	
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/*.gif "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/1/emots.txt "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1/"

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"	
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.gif "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/*.GIF "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"
	$(INSTALL_DATA) themes/emoticons/gadu-gadu/2/emots.txt "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/kadu/msg.wav"
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2"/*
