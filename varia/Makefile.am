
install-data-local: uninstall-local
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/configuration"
	$(INSTALL_DATA) configuration/*.ui "$(DESTDIR)$(datadir)/kadu/configuration"

	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/sounds/default/"
	$(INSTALL_DATA) themes/sounds/default/*.wav "$(DESTDIR)$(datadir)/kadu/themes/sounds/default"
	$(INSTALL_DATA) themes/sounds/default/sound.conf "$(DESTDIR)$(datadir)/kadu/themes/sounds/default"

	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/syntax/chat"
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/syntax/infopanel"
	$(INSTALL_DATA) syntax/chat/*.syntax "$(DESTDIR)$(datadir)/kadu/syntax/chat"
	$(INSTALL_DATA) syntax/infopanel/*.syntax "$(DESTDIR)$(datadir)/kadu/syntax/infopanel"

	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/icons/"
	@for i_t in $(enabled_icon_themes); do \
		echo -e "\033[32m>>>\033[0m Installing $$i_t icon theme ..."; \
		$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/icons/$$i_t"; \
		cd themes/icons/$$i_t; \
		i_dirs=""; \
		i_files=""; \
		for i_d in *; do \
			if test -d $$i_d; then \
				i_dirs="$$i_dirs $$i_d"; \
				cd $$i_d; \
				for ii_d in *; do \
					if test -d $$ii_d; then \
						i_dirs="$$i_dirs $$i_d/$$ii_d"; \
					fi; \
				done; \
				cd ..; \
			else \
				i_files="$$i_files $$i_d"; \
			fi; \
		done; \
		cd ../../..; \
		for i_d in $$i_dirs; do \
			$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/icons/$$i_t/$$i_d"; \
			$(INSTALL_DATA) themes/icons/$$i_t/$$i_d/* "$(DESTDIR)$(datadir)/kadu/themes/icons/$$i_t/$$i_d"; \
		done; \
		for i_f in $$i_files; do \
			$(INSTALL_DATA) themes/icons/$$i_t/$$i_f "$(DESTDIR)$(datadir)/kadu/themes/icons/$$i_t"; \
		done; \
	done

	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/emoticons/"
	@for e_t in $(enabled_emoticon_themes); do \
		echo -e "\033[32m>>>\033[0m Installing $$e_t emoticon theme ..."; \
		$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/emoticons/$$e_t"; \
		cd themes/emoticons/$$e_t; \
		e_dirs=""; \
		for e_d in *; do \
			if test -d $$e_d; then \
				e_dirs="$$e_dirs $$e_d"; \
			fi; \
		done; \
		cd ../../..; \
		for e_d in $$e_dirs; do \
			$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/themes/emoticons/$$e_t/$$e_d"; \
			$(INSTALL_DATA) themes/emoticons/$$e_t/$$e_d/* "$(DESTDIR)$(datadir)/kadu/themes/emoticons/$$e_t/$$e_d"; \
		done; \
		if test -z "$$e_dirs"; then \
			$(INSTALL_DATA) themes/emoticons/$$e_t/* "$(DESTDIR)$(datadir)/kadu/themes/emoticons/$$e_t/"; \
		fi; \
	done

	$(mkinstalldirs) "$(DESTDIR)$(bindir)/"
	$(INSTALL_SCRIPT) scripts/kadu-config "$(DESTDIR)$(bindir)/"

uninstall-local:
	rm -f "$(DESTDIR)$(datadir)/kadu/configuration"/*
	rm -f "$(DESTDIR)$(datadir)/kadu/themes/sounds/default"/*
	rm -f "$(DESTDIR)$(datadir)/kadu/syntax/chat"/*.syntax
	rm -f "$(DESTDIR)$(datadir)/kadu/syntax/infopanel"/*.syntax
	rm -rf "$(DESTDIR)$(datadir)/kadu/themes/icons"/*
	rm -rf "$(DESTDIR)$(datadir)/kadu/themes/emoticons"/*
	rm -f "$(DESTDIR)$(bindir)/kadu-config"

#				$(INSTALL_DATA) $$e_d/* "$(DESTDIR)$(datadir)/kadu/themes/emoticons/$$e_t/$$e_d"; \
#
