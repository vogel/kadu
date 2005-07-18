
install-data-local:
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/translations"; \
	$(INSTALL_DATA) *.qm "$(DESTDIR)$(datadir)/kadu/translations";

uninstall-local:
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/kadu_en.qm"; \
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/kadu_pl.qm"; \
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/kadu_it.qm"; \
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/kadu_de.qm"; \
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/qt_en.qm"; \
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/qt_pl.qm";
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/qt_it.qm";
	rm -f "$(DESTDIR)$(datadir)/kadu/translations/qt_de.qm";
