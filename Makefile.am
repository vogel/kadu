
SUBDIRS = @MY_SUBDIRS@

DIST_SUBDIRS = @MY_SUBDIRS@

EXTRA_DIST = admin kadu.kdevprj AUTHORS COPYING ChangeLog INSTALL README TODO kadu.lsm kadu.spec 

AUTOMAKE_OPTIONS = foreign 

install-data-local:
	@echo
	@echo " **************************************"
	@echo " * Kadu is now successfully installed *"
	@echo " **************************************"
	@echo
	@echo " Configuration file /etc/ld.so.conf must contain"
	@echo " this path: $(libdir)"
	@echo " If you have to add it there please remember to run"
	@echo " ldconfig afterwards."
	@echo
	@echo " Thanks for choosing Kadu Communicator and ENJOY!!!"
	@echo
	@echo "                                  Kadu Team"
	@echo
