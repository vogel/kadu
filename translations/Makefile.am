LOGFILE = ../logfile.txt

translation_ts_files = $(wildcard *.ts)
translation_qm_files = $(addsuffix .qm,$(basename $(translation_ts_files)))

all: $(translation_qm_files)

%.qm: %.ts
	@echo compiling translation file $<
	@echo "$(QT_LRELEASE_DIR)/lrelease $<" >> $(LOGFILE)
	@$(QT_LRELEASE_DIR)/lrelease $<

clean-local:
	rm -f *.qm

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
