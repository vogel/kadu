
bin_PROGRAMS = kadu

kadu_SOURCES = \
about.cpp about.h \
adduser.cpp adduser.h \
chat.cpp chat.h \
config.cpp config.h \
dcc.cpp dcc.h \
events.cpp events.h \
expimp.cpp expimp.h \
history.cpp history.h \
ignore.cpp ignore.h \
kadu.cpp kadu.h \
main.cpp \
message.cpp message.h \
network.cpp \
personal_info.cpp personal_info.h \
pixmaps.h \
register.cpp register.h \
search.cpp search.h \
sms.cpp sms.h \
userbox.cpp userbox.h \
userinfo.cpp userinfo.h \
userlist.cpp

METASOURCES = AUTO

INCLUDES = $(all_includes) 
kadu_LDADD = $(all_libraries) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) -L../libgadu/lib -lgadu
kadu_DEPENDENCIES = ../libgadu/lib/libgadu.a

EXTRA_DIST = hi16-app-kadu.png hi32-app-kadu.png hi48-app-kadu.png kadu.desktop

../libgadu/lib/libgadu.a:
	cd ../libgadu && $(MAKE)

install-data-local:
	if test ! -z "$(KDEDIR)"; then \
		$(mkinstalldirs) "$(KDEDIR)/share/applnk/Applications/"; \
		$(INSTALL_DATA) kadu.desktop "$(KDEDIR)/share/applnk/Applications"; \
		$(mkinstalldirs) "$(KDEDIR)/share/icons/hicolor/48x48/apps/"; \
		$(INSTALL_DATA) hi48-app-kadu.png "$(KDEDIR)/share/icons/hicolor/48x48/apps/kadu.png"; \
		$(mkinstalldirs) "$(KDEDIR)/share/icons/hicolor/32x32/apps/"; \
		$(INSTALL_DATA) hi32-app-kadu.png "$(KDEDIR)/share/icons/hicolor/32x32/apps/kadu.png"; \
		$(mkinstalldirs) "$(KDEDIR)/share/icons/hicolor/16x16/apps/"; \
		$(INSTALL_DATA) hi16-app-kadu.png "$(KDEDIR)/share/icons/hicolor/16x16/apps/kadu.png"; \
	fi;

uninstall-local:
	rm -f "$(KDEDIR)/share/applnk/Applications/kadu.desktop"
	rm -f "$(KDEDIR)/share/icons/hicolor/48x48/apps/kadu.png"
	rm -f "$(KDEDIR)/share/icons/hicolor/32x32/apps/kadu.png"
	rm -f "$(KDEDIR)/share/icons/hicolor/16x16/apps/kadu.png"
