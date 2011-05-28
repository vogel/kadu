set (buddies_SRCS
	buddy.cpp
	buddy-additional-data-delete-handler.cpp
	buddy-additional-data-delete-handler-manager.cpp
	buddy-kadu-data.cpp
	buddy-list.cpp
	buddy-list-mime-data-helper.cpp
	buddy-manager.cpp
	buddy-or-contact.cpp
	buddy-preferred-manager.cpp
	buddy-remove-predicate-object.cpp
	buddy-search-criteria.cpp
	buddy-set.cpp
	buddy-shared.cpp
	group.cpp
	group-manager.cpp
	group-shared.cpp

	filter/account-buddy-filter.cpp
	filter/anonymous-buddy-filter.cpp
	filter/anonymous-without-messages-buddy-filter.cpp
	filter/blocked-buddy-filter.cpp
	filter/buddy-name-filter.cpp
	filter/group-buddy-filter.cpp
	filter/has-description-buddy-filter.cpp
	filter/mobile-buddy-filter.cpp
	filter/non-buddy-filter.cpp
	filter/offline-buddy-filter.cpp
	filter/online-and-description-buddy-filter.cpp
	filter/pending-messages-filter.cpp

	model/abstract-buddies-model.cpp
	model/buddy-data-extractor.cpp
	model/buddy-list-model.cpp
	model/buddies-model.cpp
	model/buddies-model-base.cpp
	model/buddies-model-proxy.cpp
	model/groups-model.cpp
)

set (buddies_MOC_SRCS
	buddy-additional-data-delete-handler-manager.h
	buddy-kadu-data.h
	buddy-shared.h
	buddy-manager.h
	group-manager.h
	buddy-preferred-manager.h
	group-shared.h

	filter/abstract-buddy-filter.h
	filter/account-buddy-filter.h
	filter/anonymous-buddy-filter.h
	filter/anonymous-without-messages-buddy-filter.h
	filter/blocked-buddy-filter.h
	filter/buddy-name-filter.h
	filter/group-buddy-filter.h
	filter/has-description-buddy-filter.h
	filter/mobile-buddy-filter.h
	filter/non-buddy-filter.h
	filter/offline-buddy-filter.h
	filter/online-and-description-buddy-filter.h
	filter/pending-messages-filter.h

	model/buddy-list-model.h
	model/buddies-model.h
	model/buddies-model-base.h
	model/buddies-model-proxy.h
	model/groups-model.h
)

qt4_wrap_cpp (buddies_MOC_FILES ${buddies_MOC_SRCS})

add_library (buddies
	STATIC
	${buddies_SRCS}
	${buddies_MOC_FILES}
)

set_target_properties (buddies PROPERTIES COMPILE_DEFINITIONS KADULIB)
