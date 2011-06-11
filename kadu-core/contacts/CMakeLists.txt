set (contacts_SRCS
	contact.cpp
	contact-details.cpp
	contact-manager.cpp
	contact-parser-tags.cpp
	contact-set.cpp
	contact-set-configuration-helper.cpp
	contact-shared.cpp

	filter/contact-no-unloaded-account-filter.cpp

	model/buddy-contact-model.cpp
	model/contact-data-extractor.cpp
	model/contact-list-model.cpp
)

set (contacts_MOC_SRCS
	contact-details.h
	contact-manager.h
	contact-shared.h

	filter/abstract-contact-filter.h
	filter/contact-no-unloaded-account-filter.h

	model/buddy-contact-model.h
	model/contact-list-model.h
)

qt4_wrap_cpp (contacts_MOC_FILES ${contacts_MOC_SRCS})

add_library (contacts
	STATIC
	${contacts_SRCS}
	${contacts_MOC_FILES}
)

set_target_properties (contacts PROPERTIES COMPILE_DEFINITIONS KADULIB)
