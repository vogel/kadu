
bin_PROGRAMS = kadu

kadu_SOURCES = \
about.cpp about.h about_moc.cpp \
chat.cpp chat.h chat_moc.cpp \
config_dialog.cpp config_dialog.h config_dialog_moc.cpp \
config_file.cpp config_file.h \
dcc.cpp dcc.h dcc_moc.cpp \
debug.cpp debug.h \
dock_widget.cpp dock_widget.h dock_widget_moc.cpp \
emoticons.cpp emoticons.h emoticons_moc.cpp \
events.cpp events.h events_moc.cpp \
expimp.cpp expimp.h expimp_moc.cpp \
gadu.cpp gadu.h gadu_moc.cpp \
hints.cpp hints.h hints_moc.cpp \
history.cpp history.h history_moc.cpp \
ignore.cpp ignore.h ignore_moc.cpp \
kadu.cpp kadu.h kadu_moc.cpp \
main.cpp \
message_box.cpp message_box.h message_box_moc.cpp \
misc.cpp misc.h misc_moc.cpp \
password.cpp password.h password_moc.cpp \
pending_msgs.cpp pending_msgs.h pending_msgs_moc.cpp \
personal_info.cpp personal_info.h personal_info_moc.cpp \
register.cpp register.h register_moc.cpp \
search.cpp search.h search_moc.cpp \
sms.cpp sms.h sms_moc.cpp \
status.cpp status.h status_moc.cpp \
tabbar.cpp tabbar.h tabbar_moc.cpp \
userbox.cpp userbox.h userbox_moc.cpp \
userinfo.cpp userinfo.h userinfo_moc.cpp \
userlist.cpp userlist.h userlist_moc.cpp \
updates.cpp updates.h

EXTRA_kadu_SOURCES = \
modules.cpp modules.h modules_moc.cpp \
voice.cpp voice.h voice_moc.cpp \
voice_dsp.cpp voice_dsp.h voice_dsp_moc.cpp \
simlite.c simlite.h

if MODULES
MODULES_OBJS = \
modules.$(OBJEXT) modules_moc.$(OBJEXT)
else
MODULES_OBJS =
endif

if VOICE
VOICE_OBJS = \
voice.$(OBJEXT) voice_moc.$(OBJEXT) \
voice_dsp.$(OBJEXT) voice_dsp_moc.$(OBJEXT)
else
VOICE_OBJS =
endif

if OPENSSL
SIM = simlite.$(OBJEXT)
else
SIM =
endif

#kadu_METASOURCES = AUTO

INCLUDES = $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GSM_INCLUDES) $(GADU_INCLUDES)
kadu_LDADD = $(MODULES_OBJS) $(VOICE_OBJS) $(SIM) $(PTHREAD_LIBS) $(PNG_LIBS) $(QT_LIBS) $(GSM_LIBS) $(GADU_LIBS)
kadu_LDFLAGS = $(PTHREAD_LDFLAGS) $(MODULES_LDFLAGS) $(OPENSSL_LIBS)
kadu_DEPENDENCIES = $(MODULES_OBJS) $(VOICE_OBJS) $(SIM)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT \
-DBINDIR="\"$(bindir)\"" -DDATADIR="\"$(datadir)\""

EXTRA_DIST = hi16-app-kadu.png hi32-app-kadu.png hi48-app-kadu.png kadu.desktop
CLEANFILES = *_moc.cpp

%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<

install-data-local:
	$(mkinstalldirs) "$(includedir)/kadu/"
	$(INSTALL_DATA) *.h "$(includedir)/kadu/"
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
	rm -f "$(includedir)/kadu"/*.h
	rm -f "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-48.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-16.png"
