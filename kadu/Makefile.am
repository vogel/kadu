
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
misc.cpp misc.h \
network.cpp \
pending_msgs.cpp pending_msgs.h \
personal_info.cpp personal_info.h \
pixmaps.h \
register.cpp register.h \
search.cpp search.h \
sms.cpp sms.h \
userbox.cpp userbox.h \
userinfo.cpp userinfo.h \
userlist.cpp userlist.h \
vuser.cpp vuser.h \
dock_widget.cpp dock_widget.h \
emoticons.cpp emoticons.h \
status.cpp status.h \
updates.cpp updates.h

EXTRA_kadu_SOURCES = sim.cpp sim.h

if OPENSSL
SIM = sim.$(OBJEXT)
else
SIM =
endif

kadu_METASOURCES = AUTO

INCLUDES = $(all_includes)
kadu_LDADD = $(SIM) $(all_libraries) $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) -lgadu
kadu_DEPENDENCIES = $(SIM)
AM_CPPFLAGS = -DBINDIR="\"$(bindir)\"" -DDATADIR="\"$(datadir)\""

EXTRA_DIST = hi16-app-kadu.png hi32-app-kadu.png hi48-app-kadu.png kadu.desktop

install-data-local:
	if test ! -z "$(KDEDIR)"; then \
		$(mkinstalldirs) "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/"; \
		echo "[Desktop Entry]" > "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "Type=Application" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "Exec=$(bindir)/kadu" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "Icon=kadu.png" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "MiniIcon=kadu.png" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "Terminal=0" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		echo "Name=Kadu" >> "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"; \
		$(mkinstalldirs) "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/48x48/apps/"; \
		$(INSTALL_DATA) hi48-app-kadu.png "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/48x48/apps/kadu.png"; \
		$(mkinstalldirs) "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/32x32/apps/"; \
		$(INSTALL_DATA) hi32-app-kadu.png "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/32x32/apps/kadu.png"; \
		$(mkinstalldirs) "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/16x16/apps/"; \
		$(INSTALL_DATA) hi16-app-kadu.png "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/16x16/apps/kadu.png"; \
	fi;

uninstall-local:
	rm -f "$(DESTDIR)/$(KDEDIR)/share/applnk/Applications/kadu.desktop"
	rm -f "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/48x48/apps/kadu.png"
	rm -f "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/32x32/apps/kadu.png"
	rm -f "$(DESTDIR)/$(KDEDIR)/share/icons/hicolor/16x16/apps/kadu.png"
