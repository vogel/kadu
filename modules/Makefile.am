
INCLUDES = -I.. -I../kadu $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GSM_INCLUDES) $(GADU_INCLUDES)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT

CLEANFILES = */*.so */*.a */*.o */*_moc.cpp */*.d

all:
	@for dir in $(static_modules) $(shared_modules); do \
		MODULE_SOURCES= && \
		MODULE_MOC_HEADERS= && \
		MODULE_HEADERS= && \
		MODULE_CXXFLAGS= && \
		MODULE_LDFLAGS= && \
		. ./$$dir/spec && \
		MODULE_CXXFLAGS="$$MODULE_CXXFLAGS "`cat $$dir/cxxflags` && \
		MODULE_LDFLAGS="$$MODULE_LDFLAGS "`cat $$dir/ldflags` && \
		echo && \
		echo -e "\033[32m>>>\033[0m Compiling $$dir module [LD=$$MODULE_LDFLAGS,CXX=$$MODULE_CXXFLAGS] ..." && \
		echo && \
		moc_cpp_files="" && \
		o_files="" && \
		h_files="" && \
		for src_file in $$MODULE_SOURCES; do \
			o_file=`expr $$src_file : "\(.*\)\\.c.*"`.o && \
			o_files="$$o_files $$dir/$$o_file"; \
		done && \
		for moc_h_file in $$MODULE_MOC_HEADERS; do \
			h_files="$$h_files $$dir/$$moc_h_file" && \
			moc_cpp_file=`expr $$moc_h_file : "\(.*\).h"`_moc.cpp && \
			moc_cpp_files="$$moc_cpp_files $$dir/$$moc_cpp_file" && \
			o_file=`expr $$moc_h_file : "\(.*\).h"`_moc.o && \
			o_files="$$o_files $$dir/$$o_file"; \
		done && \
		for h_file in $$MODULE_HEADERS; do \
			h_files="$$h_files $$dir/$$h_file"; \
		done && \
		file_name=$$dir.so && \
		if test -z `expr " $(shared_modules) " : ".* \($$dir\) "`; then \
			file_name=$$dir.a; \
		fi && \
		$(MAKE) "module_o_files=$$o_files" \
			"module_h_files=$$h_files" \
			"module_moc_cpp_files=$$moc_cpp_files" \
			"module_cxxflags=$$MODULE_CXXFLAGS" \
			"module_ldflags=$$MODULE_LDFLAGS" \
			$$dir/$$file_name || \
		exit 1; \
	done && \
	echo && \
	echo -e "\033[32m>>>\033[0m Modules successfully compiled." && \
	echo

module_d_files = $(addsuffix .d,$(basename $(module_o_files)))

# dirty hack - leading space required becouse od automake "if" support
# time to say automake goodbye?
 ifneq (,$(module_d_files))
-include $(module_d_files)
 endif

# this empty targets are specified to mark these files are not-intermediate
$(module_o_files):
$(module_moc_cpp_files):

%.so: $(module_o_files)
	$(CC) -shared -o $@ $(module_o_files) $(module_ldflags)

%.a: $(module_o_files)
	$(AR) rs $@ $(module_o_files)

%.d: %.cpp
	$(CC) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $< | \
	sed "s,\(.*\):,$(basename $@).o $@:," > $@

%.d: %.c
	$(CC) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $< | \
	sed "s,\(.*\):,$(basename $@).o $@:," > $@

%.o: %.cpp
	$(CC) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)

%.o: %.c
	$(CC) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)

%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<


install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(includedir)/kadu/modules/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/modules/"
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/modules/translations"
	@for dir in $(static_modules) $(shared_modules); do \
		echo -e "\033[32m>>>\033[0m Installing $$dir module ..."; \
		$(INSTALL_DATA) $$dir/$$dir.desc "$(DESTDIR)/$(datadir)/kadu/modules"; \
		$(INSTALL_DATA) $$dir/*.h "$(DESTDIR)/$(includedir)/kadu/modules"; \
		if test -d $$dir/translations; then \
			$(INSTALL_DATA) $$dir/translations/*.qm "$(DESTDIR)/$(datadir)/kadu/modules/translations/"; \
		fi; \
		if test -z `expr " $(static_modules) " : ".* \($$dir\) "`; then \
			$(INSTALL_DATA) $$dir/$$dir.so "$(DESTDIR)/$(datadir)/kadu/modules/"; \
		fi; \
	done

uninstall-local:
	rm -f "$(DESTDIR)/$(includedir)/kadu/modules/*.h"
	rm -f $(DESTDIR)/$(datadir)/kadu/modules/*.desc
	rm -f $(DESTDIR)/$(datadir)/kadu/modules/*.so
	rm -f $(DESTDIR)/$(datadir)/kadu/modules/translations/*.qm
