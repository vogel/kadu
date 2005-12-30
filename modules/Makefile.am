
###########################################################################
# Precompiled headers support
###########################################################################

if PHEADERS_ENABLED
PHEADERS_INCLUDES = -include kadu-headers.h
endif

###########################################################################

LOGFILE = ../logfile.txt

CONFIGURE_INCLUDES = $(XFREE_INCLUDES) $(QT_INCLUDES) $(OPENSSL_INCLUDES) $(GSM_INCLUDES) $(GADU_INCLUDES)
INCLUDES = $(PHEADERS_INCLUDES) -I.. -I../kadu $(CONFIGURE_INCLUDES)

AM_CPPFLAGS = $(PTHREAD_CPPFLAGS) -DQT_THREAD_SUPPORT -D_REENTRANT

CLEANFILES = */*.$(SO_EXT) */*.a */*.o */*_moc.cpp */*.d

CXXFLAGS = @CXXFLAGS@

CFLAGS = @CFLAGS@

module_d_files = $(addsuffix .d,$(basename $(module_o_files)))

# dirty hack - leading space required because of automake "if" support
# time to say automake goodbye?
 ifeq ($(enable_dependency_tracking),yes)
 ifneq (,$(module_d_files))
 ifeq ($(enable_final),yes)
-include $(module_name)/.final.d
 else
-include $(module_d_files)
 endif
 endif
 endif

# this empty targets are specified to mark these files are not-intermediate
$(module_o_files):
$(module_src_files):
$(module_moc_cpp_files):

 ifeq ($(enable_final),yes)

%.$(SO_EXT): $(module_src_files) $(module_moc_cpp_files) $(module_h_files) $(module_name)/.final.d
	@dir="$$module_name" && \
	fincpp="$$dir/.final.cpp" && \
	finc="$$dir/.final.c" && \
	if test -f $$finc; then \
		echo compiling c files for module $$dir && \
		echo "$(CC) -o $$dir/.final.c.o -c -fPIC $$finc $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS)" >> $(LOGFILE) && \
		$(CC) -o $$dir/.final.c.o -c -fPIC $$finc $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS); \
		fincpp="$$fincpp $$dir/.final.c.o"; \
	fi && \
	echo compiling and linking module $$dir && \
	echo "$(CXX) $(SHARED_OPT) -o $@ -fPIC $$fincpp $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $(module_ldflags) $(SHARED_FLAGS)" >> $(LOGFILE) && \
	$(CXX) $(SHARED_OPT) -o $@ -fPIC $$fincpp $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $(module_ldflags) $(SHARED_FLAGS)

%.a: $(module_src_files) $(module_moc_cpp_files) $(module_h_files) $(module_name)/.final.d
	@dir="$$module_name" && \
	fincpp="$$dir/.final.cpp" && \
	finc="$$dir/.final.c" && \
	fino="$$dir/.final.cpp.o" && \
	if test -f $$finc; then \
		echo compiling c files for module $$dir && \
		echo "$(CC) -o $$dir/.final.c.o -c -fPIC $$finc $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CFLAGS)" >> $(LOGFILE) && \
		$(CC) -o $$dir/.final.c.o -c -fPIC $$finc $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CFLAGS); \
		fino="$$fino $$dir/.final.c.o"; \
	fi && \
	echo compiling static module $$dir && \
	echo "$(CXX) -c -o $$dir/.final.cpp.o -fPIC $$fincpp $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS)" >> $(LOGFILE) && \
	$(CXX) -c -o $$dir/.final.cpp.o -fPIC $$fincpp $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) && \
	echo "$(AR) rs $@ $$fino" >> $(LOGFILE) && \
	$(AR) rs $@ $$fino

 else

%.$(SO_EXT): $(module_o_files)
	@echo linking module $@
	@echo "$(CXX) $(SHARED_OPT) -o $@ $(module_o_files) $(module_ldflags) $(SHARED_FLAGS)" >> $(LOGFILE)
	@$(CXX) $(SHARED_OPT) -o $@ $(module_o_files) $(module_ldflags) $(SHARED_FLAGS)

%.a: $(module_o_files)
	@echo creating archive for static module $@
	@echo "$(AR) rs $@ $(module_o_files)" >> $(LOGFILE)
	@$(AR) rs $@ $(module_o_files)

 endif

%.d: %.cpp
	@echo generating dependency file for $<
	@echo '$(CXX) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).o $@:," > $@' >> $(LOGFILE)
	@$(CXX) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).o $@:," > $@

%.d: %.c
	@echo generating dependency file for $<
	@echo '$(CC) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)  $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).o $@:," > $@' >> $(LOGFILE)
	@$(CC) -M $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)  $(CXXFLAGS) $< | sed "s,\(.*\):,$(basename $@).o $@:," > $@

%.o: %.cpp
	@echo compiling file $<
	@echo "$(CXX) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)  $(CXXFLAGS)" >> $(LOGFILE)
	@$(CXX) -o $@ -c -fPIC $< $(INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags)  $(CXXFLAGS)

%.o: %.c
	@echo compiling file $<
	@echo "$(CC) -o $@ -c -fPIC $< $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CFLAGS)" >> $(LOGFILE)
	$(CC) -o $@ -c -fPIC $< $(CONFIGURE_INCLUDES) $(AM_CPPFLAGS) $(module_cxxflags) $(CFLAGS)

%_moc.cpp: %.h
	@echo generating $@
	@echo "$(QT_MOC_DIR)/moc -o $@ $<" >> $(LOGFILE)
	@$(QT_MOC_DIR)/moc -o $@ $<

make_module_dirs:
	$(mkinstalldirs) "$(DESTDIR)$(includedir)/kadu/modules/"
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/modules/"
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/modules/translations/"
	$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/modules/data/"
	$(mkinstalldirs) "$(DESTDIR)$(libdir)/kadu/modules/bin/"

install-data-local: uninstall-local make_module_dirs $(install_targets)

$(install_targets):
	@dir=`echo $@ | sed s/install_//` && \
	echo -e "\033[32m>>>\033[0m Installing $$dir module ..."; \
	$(INSTALL_DATA) $$dir/$$dir.desc "$(DESTDIR)$(datadir)/kadu/modules"; \
	$(INSTALL_DATA) $$dir/*.h "$(DESTDIR)$(includedir)/kadu/modules"; \
	if test -d $$dir/translations; then \
		$(INSTALL_DATA) $$dir/translations/*.qm "$(DESTDIR)$(datadir)/kadu/modules/translations/"; \
	fi; \
	if test -d $$dir/data; then \
		for sub_dir in `find $$dir/data -type d | grep -v /CVS | grep -v /.svn`; do \
			dst_dir=`echo "$$sub_dir" | sed "s,/data/,/," | sed "s,/data$$,,"`; \
			$(mkinstalldirs) "$(DESTDIR)$(datadir)/kadu/modules/data/$$dst_dir"; \
			for new_file in `find $$sub_dir -maxdepth 1 -type f | sed "s/ /_SPACE_/g"`; do \
				new_file=`echo "$$new_file" | sed "s/_SPACE_/ /g"`; \
				$(INSTALL_DATA) "$$new_file" "$(DESTDIR)$(datadir)/kadu/modules/data/$$dst_dir"; \
			done; \
		done; \
	fi; \
	if test -d $$dir/bin; then \
		for sub_dir in `find $$dir/bin -type d | grep -v /CVS | grep -v /.svn`; do \
			dst_dir=`echo "$$sub_dir" | sed "s,/bin/,/," | sed "s,/bin$$,,"`; \
			$(mkinstalldirs) "$(DESTDIR)$(libdir)/kadu/modules/bin/$$dst_dir"; \
			for new_file in `find $$sub_dir -maxdepth 1 -type f | sed "s/ /_SPACE_/g"`; do \
				new_file=`echo "$$new_file" | sed "s/_SPACE_/ /g"`; \
				$(INSTALL_PROGRAM) "$$new_file" "$(DESTDIR)$(libdir)/kadu/modules/bin/$$dst_dir"; \
			done; \
		done; \
	fi; \
	if test -z `expr " $(static_modules) " : ".* \($$dir\) "`; then \
		$(INSTALL_DATA) $$dir/$$dir.$(SO_EXT) "$(DESTDIR)$(libdir)/kadu/modules/"; \
	fi


uninstall-local:
	rm -f "$(DESTDIR)$(includedir)/kadu/modules"/*.h
	rm -f "$(DESTDIR)$(datadir)/kadu/modules"/*.desc
	rm -f "$(DESTDIR)$(libdir)/kadu/modules"/*.$(SO_EXT)
	rm -f "$(DESTDIR)$(datadir)/kadu/modules/translations"/*.qm
	rm -rf "$(DESTDIR)$(datadir)/kadu/modules/data"/*
	rm -rf "$(DESTDIR)$(libdir)/kadu/modules/bin"/*

clean-local:
	@for dir in $(static_modules) $(shared_modules); do \
		MODULE_3RDPARTY= && \
		. ./$$dir/spec && \
		for mod_3rdparty in $$MODULE_3RDPARTY; do \
			$(MAKE) -C $$dir/$$mod_3rdparty clean; \
		done; \
		if test -d $$dir/translations; then \
			rm -f $$dir/translations/*.qm; \
		fi; \
	done

module_targets = $(addprefix module_,$(shared_modules) $(static_modules))
install_targets = $(addprefix install_,$(shared_modules) $(static_modules))

$(module_targets):
	@dir=`echo $@ | sed s/module_//` && \
	echo && \
	echo -e "\033[32m>>>\033[0m Compiling $$dir module ..." && \
	MODULE_SOURCES= && \
	MODULE_MOC_HEADERS= && \
	MODULE_HEADERS= && \
	MODULE_CXXFLAGS= && \
	MODULE_LDFLAGS= && \
	MODULE_3RDPARTY= && \
	MODULE_MAKE_CMD= && \
	MODULE_DIR=../modules/$$dir && \
	. ./$$dir/spec && \
	MODULE_CXXFLAGS=`cat $$dir/.cxxflags` && \
	MODULE_LDFLAGS=`cat $$dir/.ldflags` && \
	echo -e "    [LDFLAGS=\"$$MODULE_LDFLAGS\", CXXFLAGS=\"$$MODULE_CXXFLAGS\"]" && \
	echo && \
	for mod_3rdparty in $$MODULE_3RDPARTY; do \
		$(MAKE) -C $$dir/$$mod_3rdparty CC="$(CC)" CXX="$(CXX)"; \
	done && \
	moc_cpp_files="" && \
	o_files="" && \
	h_files="" && \
	src_files="" && \
	for src_file in $$MODULE_SOURCES; do \
		src_files="$$src_files $$dir/$$src_file"; \
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
	file_name=$$dir.$(SO_EXT) && \
	if test -z `expr " $(shared_modules) " : ".* \($$dir\) "`; then \
		file_name=$$dir.a; \
	fi && \
	$(MAKE) "module_name=$$dir" \
		"module_o_files=$$o_files" \
		"module_src_files=$$src_files" \
		"module_h_files=$$h_files" \
		"module_moc_cpp_files=$$moc_cpp_files" \
		"module_cxxflags=$$MODULE_CXXFLAGS" \
		"module_ldflags=$$MODULE_LDFLAGS" \
		$$dir/$$file_name && \
	cd $$dir && eval "$$MODULE_MAKE_CMD" && \
	if test -d translations; then \
		cd translations && \
		for trans in *.ts; do \
			qm=`echo $${trans} | sed "s/\.ts/\.qm/"` && \
			if [ ! -e $${qm} -o $$trans -nt $${qm} ]; then \
				echo compiling translation file $$trans; \
				echo $(QT_LRELEASE_DIR)/lrelease $$trans >> ../../$(LOGFILE); \
				$(QT_LRELEASE_DIR)/lrelease $$trans; \
			fi; \
		done && \
		cd ..; \
	fi && \
	cd .. || \
	exit 1

all: $(module_targets)
