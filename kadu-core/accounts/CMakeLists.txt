set (accounts_SRCS
	account.cpp
	account_data.cpp
	account_manager.cpp
)

set (accounts_MOC_SRCS
	account.h
	account_data.h
	account_manager.h
)

qt_wrap_cpp (accounts MOC_FILES ${accounts_MOC_SRCS})
add_library (accounts STATIC ${accounts_SRCS} ${MOC_FILES})

set_target_properties(accounts PROPERTIES COMPILE_FLAGS "-DKADULIB")
