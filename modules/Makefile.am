
INCLUDES = -I../kadu $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GSM_INCLUDES) $(GADU_INCLUDES)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT \
-DBINDIR="\"$(bindir)\"" -DDATADIR="\"$(datadir)\""

CLEANFILES = *.so *.o *_moc.cpp

module_spec_list = $(wildcard */spec)
module_dir_list = $(patsubst %/spec,%,$(module_spec_list))
module_so_list = $(foreach dir,$(module_dir_list),$(dir)/$(dir).so)

all:
	@for dir in $(module_dir_list); do \
		MODULE_SOURCES=; \
		MODULE_MOC_HEADERS=; \
		MODULE_HEADERS=; \
		MODULE_CXXFLAGS=; \
		MODULE_LDFLAGS=; \
		source $$dir/spec; \
		MODULE_CXXFLAGS="$$MODULE_CXXFLAGS "`cat $$dir/cxxflags`; \
		MODULE_LDFLAGS="$$MODULE_LDFLAGS "`cat $$dir/ldflags`; \
		echo; \
		echo ">>> Compiling $$dir module [LD=$$MODULE_LDFLAGS,CXX=$$MODULE_CXXFLAGS] ..."; \
		echo; \
		obj_files=" "; \
		h_files=""; \
		for src_file in $$MODULE_SOURCES; do \
			obj_file=`expr $$src_file : "\(.*\).cpp"`.o; \
			obj_files="$$obj_files $$dir/$$obj_file"; \
		done; \
		for moc_h_file in $$MODULE_MOC_HEADERS; do \
			h_files="$$h_files $$dir/$$moc_h_file"; \
			obj_file=`expr $$moc_h_file : "\(.*\).h"`_moc.o; \
			obj_files="$$obj_files $$dir/$$obj_file"; \
		done; \
		for h_file in $$MODULE_HEADERS; do \
			h_files="$$h_files $$dir/$$h_file"; \
		done; \
		$(MAKE) "module_obj_files=$$obj_files" \
			"module_h_files=$$h_files" \
			"module_cxxflags=$$MODULE_CXXFLAGS" \
			"module_ldflags=$$MODULE_LDFLAGS" \
			$$dir/$$dir.so; \
	done

%.so: $(module_obj_files)
	$(CC) -shared -o $@ $(module_obj_files) $(module_ldflags)

%_moc.o: %_moc.cpp
	$(CC) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)

%.o: %.cpp $(module_h_files)
	$(CC) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)

%_moc.cpp: %.h
	$(QT_MOC_DIR)/moc -o $@ $<


install-data-local:
	$(mkinstalldirs) "$(DESTDIR)/$(datadir)/kadu/modules/"	
	@for dir in $(module_dir_list); do \
		$(INSTALL_DATA) $$dir/$$dir.so "$(DESTDIR)/$(datadir)/kadu/modules/"; \
	done

uninstall-local:
	rm -f $(DESTDIR)/$(datadir)/kadu/modules/*.so
