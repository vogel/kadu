
install-data-local: uninstall-local
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default/"
	$(INSTALL_DATA) themes/sounds/default/*.wav "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"
	$(INSTALL_DATA) themes/sounds/default/sound.conf "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"

	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/icons/"
	@for i_t in $(enabled_icon_themes); do \
		echo -e "\033[32m>>>\033[0m Installing $$i_t icon theme ..."; \
		$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/themes/icons/$$i_t"; \
		$(INSTALL_DATA) themes/icons/$$i_t/icons.conf "$(DESTDIR)/$(datadir)/kadu/themes/icons/$$i_t"; \
		$(INSTALL_DATA) themes/icons/$$i_t/* "$(DESTDIR)/$(datadir)/kadu/themes/icons/$$i_t"; \
	done

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
	$(INSTALL_SCRIPT) scripts/kadu-config "$(DESTDIR)/$(bindir)/"

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/sounds/default"/*
	rm -rf "$(DESTDIR)/$(datadir)/kadu/themes/icons"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/1"/*
	rm -f "$(DESTDIR)/$(datadir)/kadu/themes/emoticons/gadu-gadu/2"/*
	rm -f "$(DESTDIR)/$(bindir)/kadu-mozilla"
	rm -f "$(DESTDIR)/$(bindir)/kadu-config"
