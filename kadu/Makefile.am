
bin_PROGRAMS = kadu

kadu_SOURCES = \
about.cpp about.h \
chat.cpp chat.h \
config_dialog.cpp config_dialog.h \
config_file.cpp config_file.h \
debug.cpp debug.h \
emoticons.cpp emoticons.h \
expimp.cpp expimp.h \
gadu.cpp gadu.h \
history.cpp history.h \
ignore.cpp ignore.h \
kadu.cpp kadu.h \
main.cpp \
message_box.cpp message_box.h \
misc.cpp misc.h \
modules.cpp modules.h \
pending_msgs.cpp pending_msgs.h \
personal_info.cpp personal_info.h \
search.cpp search.h \
status.cpp status.h \
tabbar.cpp tabbar.h \
userbox.cpp userbox.h \
userinfo.cpp userinfo.h \
userlist.cpp userlist.h \
updates.cpp updates.h \
about_moc.cpp chat_moc.cpp config_dialog_moc.cpp emoticons_moc.cpp \
expimp_moc.cpp gadu_moc.cpp history_moc.cpp ignore_moc.cpp \
kadu_moc.cpp message_box_moc.cpp misc_moc.cpp modules_moc.cpp \
pending_msgs_moc.cpp personal_info_moc.cpp search_moc.cpp status_moc.cpp \
tabbar_moc.cpp userbox_moc.cpp userinfo_moc.cpp userlist_moc.cpp \
updates_moc.cpp

###########################################################################
# Precompiled headers support
###########################################################################

if PHEADERS_ENABLED
PHEADERS_INCLUDES = -I.. -include kadu-headers.h
PHEADERS_CLEANFILES = *.h.gch
AM_CXXFLAGS = -fPIC
endif

# dirty hack - leading space required because od automake "if" support
# time to say automake goodbye?
 ifeq ($(enable_pheaders),yes)

-include kadu-headers.d

kadu-headers.d: kadu-headers.h
	$(CXX) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $< | \
	sed "s,\(.*\):,$(basename $@).h.gch $@:," > $@

%.h.gch: %.h
	rm -f $@
	$(CXX) -o $@ -c $< $(INCLUDES) $(AM_CPPFLAGS) $(AM_CXXFLAGS) $(module_cxxflags)  $(CXXFLAGS)

 endif

###########################################################################

INCLUDES = $(PHEADERS_INCLUDES) $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GADU_INCLUDES)

kadu_LDADD = \
$(foreach mod,$(basename $(static_modules)),../modules/$(mod)/$(mod).a) \
$(static_modules_ldflags) $(GADU_LIBS) \
$(PTHREAD_LIBS) $(PNG_LIBS) $(QT_LIBS)

kadu_LDFLAGS = \
$(PTHREAD_LDFLAGS) $(DYNAMIC_OPT) $(OPENSSL_LIBS)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT

EXTRA_DIST = hi16-app-kadu.png hi22-app-kadu.png hi24-app-kadu.png hi32-app-kadu.png hi48-app-kadu.png kadu.desktop
CLEANFILES = *_moc.cpp $(PHEADERS_CLEANFILES)

%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<

install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(includedir)/kadu/"
	$(INSTALL_DATA) *.h "$(DESTDIR)/$(includedir)/kadu/"
	@if test -w "$(DESTDIR)/usr/share"; then \
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
		$(INSTALL_DATA) hi24-app-kadu.png "$(DESTDIR)/usr/share/pixmaps/kadu-24.png"; \
		$(INSTALL_DATA) hi22-app-kadu.png "$(DESTDIR)/usr/share/pixmaps/kadu-22.png"; \
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
	rm -f "$(DESTDIR)/$(includedir)/kadu"/*.h
	rm -f "$(DESTDIR)/usr/share/applnk/Internet/kadu.desktop"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-48.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu.png"
	rm -f "$(DESTDIR)/usr/share/pixmaps/kadu-16.png"
