
bin_PROGRAMS = kadu

kadu_SOURCES = \
about.cpp about.h about_moc.cpp \
adduser.cpp adduser.h adduser_moc.cpp \
chat.cpp chat.h chat_moc.cpp \
config_dialog.cpp config_dialog.h config_dialog_moc.cpp \
config_file.cpp config_file.h \
dcc.cpp dcc.h dcc_moc.cpp \
debug.cpp debug.h \
dock_widget.cpp dock_widget.h dock_widget_moc.cpp \
emoticons.cpp emoticons.h emoticons_moc.cpp \
events.cpp events.h events_moc.cpp \
expimp.cpp expimp.h expimp_moc.cpp \
history.cpp history.h history_moc.cpp \
ignore.cpp ignore.h ignore_moc.cpp \
kadu.cpp kadu.h kadu_moc.cpp \
main.cpp \
message_box.cpp message_box.h message_box_moc.cpp \
misc.cpp misc.h misc_moc.cpp\
password.cpp password.h password_moc.cpp \
pending_msgs.cpp pending_msgs.h pending_msgs_moc.cpp \
personal_info.cpp personal_info.h personal_info_moc.cpp \
register.cpp register.h register_moc.cpp \
search.cpp search.h search_moc.cpp \
sms.cpp sms.h sms_moc.cpp \
sound.cpp sound.h sound_moc.cpp \
status.cpp status.h status_moc.cpp \
tabbar.cpp tabbar.h tabbar_moc.cpp \
userbox.cpp userbox.h userbox_moc.cpp \
userinfo.cpp userinfo.h userinfo_moc.cpp \
userlist.cpp userlist.h userlist_moc.cpp \
updates.cpp updates.h

EXTRA_kadu_SOURCES = simlite.c simlite.h

if OPENSSL
SIM = simlite.$(OBJEXT)
else
SIM =
endif

#kadu_METASOURCES = AUTO

INCLUDES = $(QTINCDIR) $(OPENSSL_INCLUDES)
kadu_LDADD = $(SIM) $(PTHREAD_LIBS) $(PNG_LIBS) $(QTLIBDIR) -l$(QTMT) $(LIBINTL) -lgadu
kadu_LDFLAGS = $(PTHREAD_LDFLAGS) $(OPENSSL_LDFLAGS)
kadu_DEPENDENCIES = $(SIM)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT \
-DBINDIR="\"$(bindir)\"" -DDATADIR="\"$(datadir)\"" -DDOCDIR="\"$(docdir)\""

EXTRA_DIST = hi16-app-kadu.png hi32-app-kadu.png hi48-app-kadu.png kadu.desktop
CLEANFILES = *_moc.cpp

%_moc.cpp: %.h
	$(QTMOCDIR)/moc -o $@ $<

install-data-local:
	@if test -w "/usr/share"; then \
		$(mkinstalldirs) "$(DESTDIR)/usr/share/applnk/Internet/"; \
		echo "[Desktop Entry]" > "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "Type=Application" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "Exec=$(bindir)/kadu" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "Icon=kadu.png" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "MiniIcon=kadu-16.png" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "Terminal=0" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		echo "Name=Kadu" >> "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"; \
		$(mkinstalldirs) "$(DESTDIR)/usr/share/pixmaps/"; \
		$(INSTALL_DATA) hi48-app-kadu.png "$(DESTDIR)/usr/share/pixmaps/kadu-48.png"; \
		$(INSTALL_DATA) hi32-app-kadu.png "$(DESTDIR)/usr/share/pixmaps/kadu.png"; \
		$(INSTALL_DATA) hi16-app-kadu.png "$(DESTDIR)/usr/share/pixmaps/kadu-16.png"; \
		echo ""; \
		echo "Icons installed to $(DESTDIR)/usr/share/pixmaps"; \
		echo "Link(kadu.desktop) installed to $(DESTDIR)/usr/share/applnk/Internet"; \
		echo ""; \
	else \
		echo "";\
		echo "         *******************************************************************"; \
		echo "         *                                                                 *"; \
		echo "         *          You don't have permission to write /usr/share          *"; \
		echo "         *          Icons and link(kadu.desktop) won't be installed        *"; \
		echo "         *                                                                 *"; \
		echo "         *******************************************************************"; \
		echo $(DESTDIR)"";\
	fi;
uninstall-local:
	rm -f "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-48.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-16.png"
