
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/locale"; \
	$(INSTALL_DATA) *.ts "$(DESTDIR)/$(datadir)/locale";

uninstall-local:
	rm -f "$(DESTDIR)/$(datadir)/locale/kadu_en.ts"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/kadu_pl.ts"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/qt_en.ts"; \
	rm -f "$(DESTDIR)/$(datadir)/locale/qt_pl.ts";
