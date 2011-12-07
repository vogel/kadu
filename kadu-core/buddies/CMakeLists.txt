set (buddies_SRCS
	buddy.cpp
	buddy-additional-data-delete-handler.cpp
	buddy-additional-data-delete-handler-manager.cpp
	buddy-kadu-data.cpp
	buddy-list.cpp
	buddy-list-mime-data-helper.cpp
	buddy-manager.cpp
	buddy-preferred-manager.cpp
	buddy-search-criteria.cpp
	buddy-set.cpp
	buddy-shared.cpp
	group.cpp
	group-manager.cpp
	group-shared.cpp

	filter/non-buddy-filter.cpp
	filter/offline-buddy-filter.cpp
	filter/online-and-description-buddy-filter.cpp

	model/buddy-data-extractor.cpp
	model/buddy-list-model.cpp
	model/buddies-model.cpp
	model/buddies-model-base.cpp
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
	filter/non-buddy-filter.h
	filter/offline-buddy-filter.h
	filter/online-and-description-buddy-filter.h

	model/buddy-list-model.h
	model/buddies-model.h
	model/buddies-model-base.h
	model/groups-model.h
)

kadu_subdirectory (buddies "${buddies_SRCS}" "${buddies_MOC_SRCS}" "")
