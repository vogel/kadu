bin_PROGRAMS = kadu

kadu_SOURCES = \
about.cpp \
chat.cpp \
chat_manager.cpp \
chat_manager_slots.cpp \
chat_message.cpp \
color_selector.cpp \
config_dialog.cpp \
config_file.cpp \
custom_input.cpp \
debug.cpp \
dnshandler.cpp \
emoticons.cpp \
expimp.cpp \
gadu.cpp \
groups_manager.cpp \
history.cpp \
ignore.cpp \
kadu.cpp \
kadu_splitter.cpp \
main.cpp \
message_box.cpp \
misc.cpp \
modules.cpp \
pending_msgs.cpp \
personal_info.cpp \
search.cpp \
status.cpp \
tabbar.cpp \
userbox.cpp \
userinfo.cpp \
userlist.cpp \
userlist-private.cpp \
usergroup.cpp \
userlistelement.cpp \
updates.cpp


kadudir = $(includedir)/kadu

kadu_HEADERS = \
about.h \
chat.h \
chat_manager.cpp \
chat_manager_slots.cpp \
chat_message.cpp \
color_selector.cpp \
config_dialog.h \
config_file.h \
custom_input.cpp \
debug.h \
dnshandler.h \
emoticons.h \
expimp.h \
gadu.h \
groups_manager.h \
history.h \
ignore.h \
kadu.h \
kadu_splitter.cpp \
message_box.h \
misc.h \
modules.h \
pending_msgs.h \
personal_info.h \
search.h \
status.h \
tabbar.h \
userbox.h \
userinfo.h \
userlist.h \
userlist-private.h \
usergroup.h \
userlistelement.h \
updates.h

BUILT_SOURCES = \
moc/about_moc.cpp \
moc/chat_moc.cpp \
moc/chat_manager_moc.cpp \
moc/chat_manager_slots_moc.cpp \
moc/color_selector_moc.cpp \
moc/config_dialog_moc.cpp \
moc/custom_input_moc.cpp \
moc/dnshandler_moc.cpp \
moc/emoticons_moc.cpp \
moc/expimp_moc.cpp \
moc/gadu_moc.cpp \
moc/groups_manager_moc.cpp \
moc/history_moc.cpp \
moc/ignore_moc.cpp \
moc/kadu_moc.cpp \
moc/message_box_moc.cpp \
moc/misc_moc.cpp \
moc/modules_moc.cpp \
moc/pending_msgs_moc.cpp \
moc/personal_info_moc.cpp \
moc/search_moc.cpp \
moc/status_moc.cpp \
moc/tabbar_moc.cpp \
moc/userbox_moc.cpp \
moc/userinfo_moc.cpp \
moc/userlist_moc.cpp \
moc/userlist-private_moc.cpp \
moc/usergroup_moc.cpp \
moc/userlistelement_moc.cpp \
moc/updates_moc.cpp

am_kadu_OBJECTS = \
obj/about.$(OBJEXT) \
obj/chat.$(OBJEXT) \
obj/chat_manager.$(OBJEXT) \
obj/chat_manager_slots.$(OBJEXT) \
obj/chat_message.$(OBJEXT) \
obj/color_selector.$(OBJEXT) \
obj/config_dialog.$(OBJEXT) \
obj/config_file.$(OBJEXT) \
obj/custom_input.$(OBJEXT) \
obj/debug.$(OBJEXT) \
obj/dnshandler.$(OBJEXT) \
obj/emoticons.$(OBJEXT) \
obj/expimp.$(OBJEXT) \
obj/gadu.$(OBJEXT) \
obj/groups_manager.$(OBJEXT) \
obj/history.$(OBJEXT) \
obj/ignore.$(OBJEXT) \
obj/kadu.$(OBJEXT) \
obj/kadu_splitter.$(OBJEXT) \
obj/main.$(OBJEXT) \
obj/message_box.$(OBJEXT) \
obj/misc.$(OBJEXT) \
obj/modules.$(OBJEXT) \
obj/pending_msgs.$(OBJEXT) \
obj/personal_info.$(OBJEXT) \
obj/search.$(OBJEXT) \
obj/status.$(OBJEXT) \
obj/tabbar.$(OBJEXT) \
obj/userbox.$(OBJEXT) \
obj/userinfo.$(OBJEXT) \
obj/userlist.$(OBJEXT) \
obj/userlist-private.$(OBJEXT) \
obj/usergroup.$(OBJEXT) \
obj/userlistelement.$(OBJEXT) \
obj/updates.$(OBJEXT) \
obj/about_moc.$(OBJEXT) \
obj/chat_moc.$(OBJEXT) \
obj/chat_manager_moc.$(OBJEXT) \
obj/chat_manager_slots_moc.$(OBJEXT) \
obj/color_selector_moc.$(OBJEXT) \
obj/config_dialog_moc.$(OBJEXT) \
obj/custom_input_moc.$(OBJEXT) \
obj/dnshandler_moc.$(OBJEXT) \
obj/emoticons_moc.$(OBJEXT) \
obj/expimp_moc.$(OBJEXT) \
obj/gadu_moc.$(OBJEXT) \
obj/groups_manager_moc.$(OBJEXT) \
obj/history_moc.$(OBJEXT) \
obj/ignore_moc.$(OBJEXT) \
obj/kadu_moc.$(OBJEXT) \
obj/message_box_moc.$(OBJEXT) \
obj/misc_moc.$(OBJEXT) \
obj/modules_moc.$(OBJEXT) \
obj/pending_msgs_moc.$(OBJEXT) \
obj/personal_info_moc.$(OBJEXT) \
obj/search_moc.$(OBJEXT) \
obj/status_moc.$(OBJEXT) \
obj/tabbar_moc.$(OBJEXT) \
obj/userbox_moc.$(OBJEXT) \
obj/userinfo_moc.$(OBJEXT) \
obj/userlist_moc.$(OBJEXT) \
obj/userlist-private_moc.$(OBJEXT) \
obj/usergroup_moc.$(OBJEXT) \
obj/userlistelement_moc.$(OBJEXT) \
obj/updates_moc.$(OBJEXT)

.cpp.o:
	@echo -e "\033[32m >>> make obj/"$@" <<< \033[0m"
	make obj/$@

.cpp.obj:
	@echo "go away ;)"

obj/%.o: %.cpp
@am__fastdepCXX_TRUE@	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	$(CXXDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCXX_FALSE@	$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

obj/%_moc.o: moc/%_moc.cpp
@am__fastdepCXX_TRUE@	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -MT $@ -MD -MP -MF "$(DEPDIR)/$*_moc.Tpo" -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<; then mv -f "$(DEPDIR)/$*_moc.Tpo" "$(DEPDIR)/$*_moc.Po"; else rm -f "$(DEPDIR)/$*_moc.Tpo"; exit 1; fi
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	source='$<' object='$@' libtool=no @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' @AMDEPBACKSLASH@
@AMDEP_TRUE@@am__fastdepCXX_FALSE@	$(CXXDEPMODE) $(depcomp) @AMDEPBACKSLASH@
@am__fastdepCXX_FALSE@	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

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
CLEANFILES = obj/*.o moc/*_moc.cpp $(PHEADERS_CLEANFILES)

moc/%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<

install-data-local:
#	$(mkinstalldirs) "$(DESTDIR)/$(includedir)/kadu/"
#	$(INSTALL_DATA) *.h "$(DESTDIR)/$(includedir)/kadu/"
	@if test -w "$(DESTDIR)$(datadir)"; then \
		$(mkinstalldirs) "$(DESTDIR)$(datadir)/applnk/Internet/"; \
		echo "[Desktop Entry]" > "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "Type=Application" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "Exec=$(bindir)/kadu" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "Icon=kadu.png" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "MiniIcon=kadu-16.png" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "Terminal=0" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		echo "Name=Kadu" >> "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"; \
		$(mkinstalldirs) "$(DESTDIR)$(datadir)/pixmaps/"; \
		$(INSTALL_DATA) hi48-app-kadu.png "$(DESTDIR)$(datadir)/pixmaps/kadu-48.png"; \
		$(INSTALL_DATA) hi32-app-kadu.png "$(DESTDIR)$(datadir)/pixmaps/kadu.png"; \
		$(INSTALL_DATA) hi24-app-kadu.png "$(DESTDIR)$(datadir)/pixmaps/kadu-24.png"; \
		$(INSTALL_DATA) hi22-app-kadu.png "$(DESTDIR)$(datadir)/pixmaps/kadu-22.png"; \
		$(INSTALL_DATA) hi16-app-kadu.png "$(DESTDIR)$(datadir)/pixmaps/kadu-16.png"; \
		echo ""; \
		echo "Icons installed to $(DESTDIR)$(datadir)/pixmaps"; \
		echo "Link(kadu.desktop) installed to $(DESTDIR)$(datadir)/applnk/Internet"; \
		echo ""; \
	else \
		echo "";\
		echo "         *******************************************************************"; \
		echo "         *                                                                 *"; \
		echo "         *          You don't have permission to write $(datadir)          *"; \
		echo "         *          Icons and link(kadu.desktop) won't be installed        *"; \
		echo "         *                                                                 *"; \
		echo "         *******************************************************************"; \
		echo $(DESTDIR)"";\
	fi;

uninstall-local:
	rm -f "$(DESTDIR)$(includedir)/kadu"/*.h
	rm -f "$(DESTDIR)$(datadir)/applnk/Internet/kadu.desktop"
	rm -f "$(DESTDIR)$(datadir)/pixmaps/kadu-48.png"
	rm -f "$(DESTDIR)$(datadir)/pixmaps/kadu.png"
	rm -f "$(DESTDIR)$(datadir)/pixmaps/kadu-16.png"
