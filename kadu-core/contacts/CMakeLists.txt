set (contacts_SRCS
	contact.cpp
	contact-details.cpp
	contact-manager.cpp
	contact-parser-tags.cpp
	contact-set.cpp
	contact-set-configuration-helper.cpp
	contact-shared.cpp

	model/buddy-contact-model.cpp
	model/chat-adapter.cpp
	model/contact-data-extractor.cpp
	model/contact-list-model.cpp
)

set (contacts_MOC_SRCS
	contact-manager.h
	contact-shared.h

	model/buddy-contact-model.h
	model/chat-adapter.h
	model/contact-list-model.h
)

kadu_subdirectory (contacts "${contacts_SRCS}" "${contacts_MOC_SRCS}" "")
