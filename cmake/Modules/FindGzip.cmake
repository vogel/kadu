find_program (GZIP_EXECUTABLE
	NAMES gzip
	PATH_SUFFIXES Git/bin GnuWin32/bin
	DOC "gzip command line utility"
)
mark_as_advanced (GZIP_EXECUTABLE)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Gzip REQUIRED_VARS GZIP_EXECUTABLE)
