
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/locale"; \
	$(INSTALL_DATA) *.qm "$(DESTDIR)/$(datadir)/locale";

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/locale/kadu_en.qm"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/kadu_pl.qm"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/qt_en.qm"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/qt_pl.qm";
