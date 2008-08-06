DEPS=./deps

 ifeq ($(enable_dependency_tracking),yes)

include $(DEPS)/about.d
include $(DEPS)/about_moc.d
include $(DEPS)/action.d
include $(DEPS)/action_moc.d
include $(DEPS)/activate.d
include $(DEPS)/chat_edit_box.d
include $(DEPS)/chat_manager.d
include $(DEPS)/chat_manager_moc.d
include $(DEPS)/chat_message.d
include $(DEPS)/chat_messages_view.d
include $(DEPS)/chat_widget.d
include $(DEPS)/chat_widget_moc.d
include $(DEPS)/chat_window.d
include $(DEPS)/chat_window_moc.d
include $(DEPS)/color_button.d
include $(DEPS)/color_button_moc.d
include $(DEPS)/color_selector.d
include $(DEPS)/color_selector_moc.d
include $(DEPS)/configuration_window.d
include $(DEPS)/configuration_window_moc.d
include $(DEPS)/configuration_window_widgets.d
include $(DEPS)/configuration_window_widgets_moc.d
include $(DEPS)/config_file.d
include $(DEPS)/contact.d
include $(DEPS)/custom_input.d
include $(DEPS)/custom_input_moc.d
include $(DEPS)/debug.d
include $(DEPS)/dnshandler.d
include $(DEPS)/dnshandler_moc.d
include $(DEPS)/emoticons.d
include $(DEPS)/emoticons_moc.d
include $(DEPS)/expimp.d
include $(DEPS)/expimp_moc.d
include $(DEPS)/gadu-private.d
include $(DEPS)/gadu-private_moc.d
include $(DEPS)/gadu.d
include $(DEPS)/gadu_moc.d
include $(DEPS)/gadu_images_manager.d
include $(DEPS)/groups_manager.d
include $(DEPS)/groups_manager_moc.d
include $(DEPS)/hot_key.d
include $(DEPS)/html_document.d
include $(DEPS)/http_client.d
include $(DEPS)/http_client_moc.d
include $(DEPS)/icons_manager.d
include $(DEPS)/icons_manager_moc.d
include $(DEPS)/ignore.d
include $(DEPS)/ignore_moc.d
include $(DEPS)/kadu.d
include $(DEPS)/kadu_moc.d
include $(DEPS)/kadu_main_window.d
include $(DEPS)/kadu_main_window_moc.d
include $(DEPS)/kadu_parser.d
include $(DEPS)/kadu_text_browser.d
include $(DEPS)/kadu_text_browser_moc.d
include $(DEPS)/main.d
include $(DEPS)/main_configuration_window.d
include $(DEPS)/message_box.d
include $(DEPS)/message_box_moc.d
include $(DEPS)/misc.d
include $(DEPS)/misc_moc.d
include $(DEPS)/modules.d
include $(DEPS)/modules_moc.d
include $(DEPS)/path_list_edit.d
include $(DEPS)/pending_msgs.d
include $(DEPS)/pending_msgs_moc.d
include $(DEPS)/personal_info.d
include $(DEPS)/personal_info_moc.d
include $(DEPS)/preview.d
include $(DEPS)/preview_moc.d
include $(DEPS)/protocol.d
include $(DEPS)/protocol_moc.d
include $(DEPS)/protocols_manager.d
include $(DEPS)/protocols_manager_moc.d
include $(DEPS)/search.d
include $(DEPS)/search_moc.d
include $(DEPS)/select_file.d
include $(DEPS)/select_file_moc.d
include $(DEPS)/select_font.d
include $(DEPS)/select_font_moc.d
include $(DEPS)/status.d
include $(DEPS)/status_moc.d
include $(DEPS)/status_changer.d
include $(DEPS)/status_changer_moc.d
include $(DEPS)/syntax_editor.d
include $(DEPS)/syntax_editor_moc.d
include $(DEPS)/tabbar.d
include $(DEPS)/tabbar_moc.d
include $(DEPS)/themes.d
include $(DEPS)/themes_moc.d
include $(DEPS)/toolbar.d
include $(DEPS)/toolbar_moc.d
include $(DEPS)/updates.d
include $(DEPS)/updates_moc.d
include $(DEPS)/userbox.d
include $(DEPS)/userbox_moc.d
include $(DEPS)/usergroup.d
include $(DEPS)/usergroup_moc.d
include $(DEPS)/userinfo.d
include $(DEPS)/userinfo_moc.d
include $(DEPS)/userlist-private.d
include $(DEPS)/userlist-private_moc.d
include $(DEPS)/userlist.d
include $(DEPS)/userlist_moc.d
include $(DEPS)/userlistelement.d
include $(DEPS)/userlistelement_moc.d

 endif

.cpp.o:
	@echo -e "\033[32m >>> make obj/"$@" <<< \033[0m"
	make obj/$@

.cpp.obj:
	@echo "go away ;)"


 ifeq ($(enable_dependency_tracking),yes)

obj/%.o: %.cpp
	@echo generating dependency file for $<
	@$(CXXCOMPILE) -M $< -MT "$@" > deps/`echo $@|sed "s/obj\/\(.*\).o/\1.d/"`
	@echo '$(CXXCOMPILE) -M $< -MT "$@" > deps/`echo $@|sed "s/obj\/\(.*\).o/\1.d/"`' >> $(LOGFILE)
	@echo compiling file $<
	@echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$< >> $(LOGFILE)
	@$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

obj/%_moc.o: moc/%_moc.cpp
	@echo generating dependency file for $<
	@echo '$(CXXCOMPILE) -M $< -MT "$@" > deps/`echo $@|sed "s/obj\/\(.*\).o/\1.d/"`' >> $(LOGFILE)
	@$(CXXCOMPILE) -M $< -MT "$@" > deps/`echo $@|sed "s/obj\/\(.*\).o/\1.d/"`
	@echo compiling file $<
	@echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$< >> $(LOGFILE)
	@$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

 else

obj/%.o: %.cpp
	@echo compiling file $<
	@echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$< >> $(LOGFILE)
	@$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

obj/%_moc.o: moc/%_moc.cpp
	@echo compiling file $<
	@echo $(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$< >> $(LOGFILE)
	@$(CXXCOMPILE) -c -o $@ `test -f '$<' || echo '$(srcdir)/'`$<

 endif

moc/%_moc.cpp: %.h
	@echo generating $@
	@echo $(QT_MOC_DIR)/moc -o $@ $< >> $(LOGFILE)
	@$(QT_MOC_DIR)/moc -o $@ $<

###########################################################################
# Precompiled headers support
###########################################################################

if PHEADERS_ENABLED
PHEADERS_INCLUDES = -I.. -include kadu-headers.h
PHEADERS_CLEANFILES = *.h.gch
AM_CXXFLAGS = -fPIC
kadu_ADDITIONAL_DEPS = kadu-headers.h.gch
endif

# dirty hack - leading space required because od automake "if" support
# time to say automake goodbye?
 ifeq ($(enable_pheaders),yes)

-include kadu-headers.d

kadu-headers.d: kadu-headers.h
	@echo generating dependency file for $<
	@echo "$(CXX) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).h.gch $@:," > $@" >> $(LOGFILE)
	@$(CXX) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).h.gch $@:," > $@

%.h.gch: %.h
	@echo compiling headers
	@rm -f $@
	@echo $(CXX) -o $@ -c $< $(INCLUDES) $(AM_CPPFLAGS) $(AM_CXXFLAGS) $(module_cxxflags)  $(CXXFLAGS) >> $(LOGFILE)
	@$(CXX) -o $@ -c $< $(INCLUDES) $(AM_CPPFLAGS) $(AM_CXXFLAGS) $(module_cxxflags)  $(CXXFLAGS)

 endif


###########################################################################

 ifeq ($(enable_final),yes)

kadu$(EXEEXT): $(kadu_ADDITIONAL_DEPS) $(kadu_SOURCES) $(kadu_HEADERS) $(BUILT_SOURCES) $(kadu_DEPENDENCIES)
	@rm -f kadu$(EXEEXT)
	$(CXXCOMPILE) $(AM_LDFLAGS) $(LDFLAGS) -o $@ $(kadu_LDFLAGS) final.cpp $(kadu_LDADD) $(LIBS)

 else

kadu$(EXEEXT): $(kadu_ADDITIONAL_DEPS) $(kadu_OBJECTS) $(kadu_DEPENDENCIES)
	@rm -f kadu$(EXEEXT)
	$(CXXLINK) $(kadu_LDFLAGS) $(kadu_OBJECTS) $(kadu_LDADD) $(LIBS)

 endif

 ifeq ($(snapshot),yes)
version.cpp: ../SNAPSHOT
	@echo "const char *detailed_version = \"`cat ../SNAPSHOT`\";" > $@;
 else
version.cpp: $(kadu_SOURCES) *.h ../modules/*/*.h ../modules/*/*.cpp ../libgadu/src/*.c ../libgadu/include/*.h
	@core_ver=`svnversion -n .`; \
	mod_ver=`svnversion -n ../modules`; \
	lg_ver=`svnversion -n ../libgadu`; \
	if [ "$$core_ver" != "$$mod_ver" -o "$$core_ver" != "$$lg_ver" -o "$$mod_ver" != "$$lg_ver" ]; then \
		ver="core:$$core_ver modules:$$mod_ver libgadu:$$lg_ver"; \
	else \
		ver="$$core_ver"; \
	fi; \
	echo "const char *detailed_version = \"$$ver\";" | diff -q - $@ >/dev/null 2>/dev/null || echo "const char *detailed_version = \"$$ver\";" > $@;
 endif


INCLUDES = $(PHEADERS_INCLUDES) $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GADU_INCLUDES)

kadu_LDADD = \
$(foreach mod,$(basename $(static_modules)),../modules/$(mod)/$(mod).a) \
$(static_modules_ldflags) $(GADU_LIBS) \
$(PTHREAD_LIBS) $(PNG_LIBS) $(QT_LIBS)

kadu_LDFLAGS = \
$(PTHREAD_LDFLAGS) $(DYNAMIC_OPT) $(OPENSSL_LIBS)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT -DDATADIR=\"$(datadir)\" -DLIBDIR=\"$(libdir)\" -DBINDIR=\"$(bindir)\"

CLEANFILES = obj/*.o moc/*_moc.cpp $(PHEADERS_CLEANFILES)

clean-local:
	for i in deps/*.d; do \
		echo -n > $$i; \
	done
